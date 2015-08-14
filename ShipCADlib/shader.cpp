/*##############################################################################################
 *    ShipCAD
 *    Copyright 2015, by Greg Green <ggreen@bit-builder.com>
 *    Original Copyright header below
 *
 *    This code is distributed as part of the FREE!ship project. FREE!ship is an               *
 *    open source surface-modelling program based on subdivision surfaces and intended for     *
 *    designing ships.                                                                         *
 *                                                                                             *
 *    Copyright © 2005, by Martijn van Engeland                                                *
 *    e-mail                  : Info@FREEship.org                                              *
 *    FREE!ship project page  : https://sourceforge.net/projects/freeship                      *
 *    FREE!ship homepage      : www.FREEship.org                                               *
 *                                                                                             *
 *    This program is free software; you can redistribute it and/or modify it under            *
 *    the terms of the GNU General Public License as published by the                          *
 *    Free Software Foundation; either version 2 of the License, or (at your option)           *
 *    any later version.                                                                       *
 *                                                                                             *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY          *
 *    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A          *
 *    PARTICULAR PURPOSE. See the GNU General Public License for more details.                 *
 *                                                                                             *
 *    You should have received a copy of the GNU General Public License along with             *
 *    this program; if not, write to the Free Software Foundation, Inc.,                       *
 *    59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                                    *
 *                                                                                             *
 *#############################################################################################*/

#include <stdexcept>
#include <iostream>
#include <QtGui/QOpenGLShaderProgram>

#include "shader.h"
#include "viewport.h"

using namespace std;
using namespace ShipCAD;

//////////////////////////////////////////////////////////////////////////////////////

Shader::Shader(Viewport* vp)
  : _viewport(vp), _program(0)
{
  // does nothing
}

Shader::~Shader()
{
    delete _program;
}

void Shader::initialize(const char* vertexShaderSource,
            const char* fragmentShaderSource,
            vector<string> uniforms,
            vector<string> attributes)
{
  _program = new QOpenGLShaderProgram(_viewport);
  _program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
  _program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
  if (!_program->link())
      cerr << "OpenGL Link failed - Log:\n" << _program->log().toStdString() << endl;

  addUniform("matrix");

  for (size_t i=0; i<uniforms.size(); ++i)
    addUniform(uniforms[i]);
  for (size_t i=0; i<attributes.size(); ++i)
    addAttribute(attributes[i]);
}

void Shader::addUniform(const string& name)
{
  _uniforms[name] = _program->uniformLocation(name.c_str());
  if (_uniforms[name] == -1)
    throw runtime_error("bad uniform");
}

void Shader::addAttribute(const string& name)
{
  _attributes[name] = _program->attributeLocation(name.c_str());
  if (_attributes[name] == -1)
    throw runtime_error("bad attribute");
}

void Shader::setMatrix(const QMatrix4x4& matrix)
{
  _program->setUniformValue(_uniforms["matrix"], matrix);
}

//////////////////////////////////////////////////////////////////////////////////////

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "uniform highp mat4 matrix;\n"
    "uniform lowp vec4 sourceColor;\n"
    "varying mediump vec4 color;\n"
    "void main() {\n"
    "   color = sourceColor;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying mediump vec4 color;\n"
    "void main() {\n"
    "   gl_FragColor = color;\n"
    "}\n";


LineShader::LineShader(Viewport* vp)
  : Shader(vp)
{
    vector<string> attrs;
    vector<string> unis;
    unis.push_back("sourceColor");
    attrs.push_back("posAttr");
    initialize(vertexShaderSource, fragmentShaderSource, unis, attrs);
}

void LineShader::renderPoints(QVector<QVector3D>& points, QColor color)
{
    _program->setUniformValue(_uniforms["sourceColor"],
                  color.redF(),
                  color.greenF(),
                  color.blueF(),
                  1.0f);

    GLuint posAttr = _attributes["posAttr"];

    _program->setAttributeArray(posAttr, points.constData());
    _program->enableAttributeArray(posAttr);
    glDrawArrays(GL_POINTS, 0, points.size());
    _program->disableAttributeArray(posAttr);
}

void LineShader::renderLines(QVector<QVector3D>& vertices, QColor lineColor)
{
    _program->setUniformValue(_uniforms["sourceColor"],
                  lineColor.redF(),
                  lineColor.greenF(),
                  lineColor.blueF(),
                  1.0f);

    GLuint posAttr = _attributes["posAttr"];

    _program->setAttributeArray(posAttr, vertices.constData());
    _program->enableAttributeArray(posAttr);
    glDrawArrays(GL_LINES, 0, vertices.size());
    _program->disableAttributeArray(posAttr);
}

//////////////////////////////////////////////////////////////////////////////////////

static const char* VertexShadervmShade =
        "attribute highp vec4 vertex;"
        "attribute mediump vec3 normal;"
        "uniform mediump mat4 matrix;"
        "uniform lowp vec4 sourceColor;"
        "varying mediump vec4 color;"
        "void main(void)"
        "{"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));"
        "    float angle = max(dot(normal, toLight), 0.0);"
        "    vec3 col = sourceColor.rgb;"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);"
        "    color = clamp(color, 0.0, 1.0);"
        "    gl_Position = matrix * vertex;"
        "}";

static const char* FragmentShadervmShade =
        "varying mediump vec4 color;"
        "void main(void)"
        "{"
        "    gl_FragColor = color;"
        "}";

MonoFaceShader::MonoFaceShader(Viewport* vp)
  : Shader(vp)
{
    vector<string> attrs;
    vector<string> unis;
    unis.push_back("sourceColor");
    attrs.push_back("vertex");
    attrs.push_back("normal");
    initialize(VertexShadervmShade, FragmentShadervmShade, unis, attrs);
}

void MonoFaceShader::renderMesh(QColor meshColor,
                                QVector<QVector3D>& vertices,
                                QVector<QVector3D>& normals)
{
    if (vertices.size() != normals.size())
        throw runtime_error("vertex and normal array not same size MonoFaceShader::renderMesh");

    _program->setUniformValue(_uniforms["sourceColor"],
                  meshColor.redF(),
                  meshColor.greenF(),
                  meshColor.blueF(),
                  1.0f);

    GLuint normalAttr = _attributes["normal"];
    GLuint vertexAttr = _attributes["vertex"];

    _program->setAttributeArray(vertexAttr, vertices.constData());
    _program->setAttributeArray(normalAttr, normals.constData());
    _program->enableAttributeArray(normalAttr);
    _program->enableAttributeArray(vertexAttr);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    _program->disableAttributeArray(normalAttr);
    _program->disableAttributeArray(vertexAttr);
}

//////////////////////////////////////////////////////////////////////////////////////
