/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "pch.h"
#include <moaicore/MOAIBox2DDebugDraw.h>

SUPPRESS_EMPTY_FILE_WARNING
#if USE_BOX2D

#include <cstdio>
#include <cstdarg>
#include <cstring>

//================================================================//
// MOAIBox2DDebugDraw
//================================================================//

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::BindVertexFormat ( USDrawBuffer& drawBuffer ) {

	drawBuffer.BindVertexPreset ( USVertexFormatMgr::VTX_FMT_XYC );
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	USDrawBuffer& drawBuffer = USDrawBuffer::Get ();
	this->BindVertexFormat ( drawBuffer );
	
	drawBuffer.SetPenColor ( color.r, color.g, color.b, 1.0f );

	drawBuffer.BeginPrim ( GL_LINE_LOOP );
	for ( int32 i = 0; i < vertexCount; ++i ) {
		this->WriteVtx ( drawBuffer, vertices [ i ].x, vertices [ i ].y );
	}
	drawBuffer.EndPrim ();
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	USDrawBuffer& drawBuffer = USDrawBuffer::Get ();
	this->BindVertexFormat ( drawBuffer );
	
	drawBuffer.SetBlendMode ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	drawBuffer.SetPenColor ( 0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f );

	drawBuffer.BeginPrim ( GL_TRIANGLE_FAN );
	for ( int32 i = 0; i < vertexCount; ++i ) {
		this->WriteVtx ( drawBuffer, vertices [ i ].x, vertices [ i ].y );
	}
	drawBuffer.EndPrim ();
	
	drawBuffer.SetBlendMode ();
	drawBuffer.SetPenColor ( color.r, color.g, color.b, 1.0f );
	
	drawBuffer.BeginPrim ( GL_LINE_LOOP );
	for (int32 i = 0; i < vertexCount; ++i) {
		this->WriteVtx ( drawBuffer, vertices [ i ].x, vertices [ i ].y );
	}
	drawBuffer.EndPrim ();
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	USDrawBuffer& drawBuffer = USDrawBuffer::Get ();
	this->BindVertexFormat ( drawBuffer );
	
	drawBuffer.SetPenColor ( color.r, color.g, color.b, 1.0f );

	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	
	drawBuffer.BeginPrim ( GL_LINE_LOOP );
	float32 theta = 0.0f;
	for (int32 i = 0; i < k_segments; ++i) {
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		this->WriteVtx ( drawBuffer, v.x, v.y );
		theta += k_increment;
	}
	drawBuffer.EndPrim ();
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	USDrawBuffer& drawBuffer = USDrawBuffer::Get ();
	this->BindVertexFormat ( drawBuffer );
	
	drawBuffer.SetBlendMode ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	drawBuffer.SetPenColor ( 0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f );

	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	
	drawBuffer.BeginPrim ( GL_TRIANGLE_FAN );
	float32 theta = 0.0f;
	for (int32 i = 0; i < k_segments; ++i) {
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		this->WriteVtx ( drawBuffer, v.x, v.y );
		theta += k_increment;
	}
	drawBuffer.EndPrim ();

	drawBuffer.SetBlendMode ();
	drawBuffer.SetPenColor ( color.r, color.g, color.b, 1.0f );

	drawBuffer.BeginPrim ( GL_LINE_LOOP );
	theta = 0.0f;
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		this->WriteVtx ( drawBuffer, v.x, v.y );
		theta += k_increment;
	}
	drawBuffer.EndPrim ();

	b2Vec2 p = center + radius * axis;
	drawBuffer.BeginPrim ( GL_LINES );
	this->WriteVtx ( drawBuffer, center.x, center.y );
	this->WriteVtx ( drawBuffer, p.x, p.y );
	drawBuffer.EndPrim ();
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	USDrawBuffer& drawBuffer = USDrawBuffer::Get ();
	this->BindVertexFormat ( drawBuffer );
	
	drawBuffer.SetPenColor ( color.r, color.g, color.b, 1.0f );

	drawBuffer.BeginPrim ( GL_LINES );
	this->WriteVtx ( drawBuffer, p1.x, p1.y );
	this->WriteVtx ( drawBuffer, p2.x, p2.y );
	drawBuffer.EndPrim ();
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::DrawTransform(const b2Transform& xf)
{
	USDrawBuffer& drawBuffer = USDrawBuffer::Get ();
	this->BindVertexFormat ( drawBuffer );

	b2Vec2 p1 = xf.position, p2;
	const float32 k_axisScale = 0.4f;
	
	drawBuffer.BeginPrim(GL_LINES);
	
		drawBuffer.SetPenColor(1.0f, 0.0f, 0.0f, 1.0f);
		this->WriteVtx(drawBuffer, p1.x, p1.y);
		
		p2 = p1 + k_axisScale * xf.R.col1;
		this->WriteVtx(drawBuffer, p2.x, p2.y);

	drawBuffer.EndPrim();

	drawBuffer.BeginPrim(GL_LINES);
	
		drawBuffer.SetPenColor(0.0f, 1.0f, 0.0f, 1.0f);
		this->WriteVtx(drawBuffer, p1.x, p1.y);
		
		p2 = p1 + k_axisScale * xf.R.col2;
		this->WriteVtx(drawBuffer, p2.x, p2.y);

	drawBuffer.EndPrim();
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	USDrawBuffer& drawBuffer = USDrawBuffer::Get ();
	this->BindVertexFormat ( drawBuffer );

	drawBuffer.SetPointSize(size);
	drawBuffer.BeginPrim(GL_POINTS);
	drawBuffer.SetPenColor(color.r, color.g, color.b, 1.0f);
	this->WriteVtx(drawBuffer, p.x, p.y);
	drawBuffer.EndPrim();
	drawBuffer.SetPointSize(1.0f);
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	USDrawBuffer& drawBuffer = USDrawBuffer::Get ();
	this->BindVertexFormat ( drawBuffer );

	drawBuffer.SetPenColor(c.r, c.g, c.b, 1.0f);
	drawBuffer.BeginPrim(GL_LINE_LOOP);
	this->WriteVtx(drawBuffer, aabb->lowerBound.x, aabb->lowerBound.y);
	this->WriteVtx(drawBuffer, aabb->upperBound.x, aabb->lowerBound.y);
	this->WriteVtx(drawBuffer, aabb->upperBound.x, aabb->upperBound.y);
	this->WriteVtx(drawBuffer, aabb->lowerBound.x, aabb->upperBound.y);
	drawBuffer.EndPrim();
}

//----------------------------------------------------------------//
void MOAIBox2DDebugDraw::WriteVtx ( USDrawBuffer& drawBuffer, float x, float y ) {

	USVec2D vtx;
	vtx.mX = x;
	vtx.mY = y;
	drawBuffer.WriteVtx ( vtx );
	drawBuffer.WritePenColor ();
}

#endif