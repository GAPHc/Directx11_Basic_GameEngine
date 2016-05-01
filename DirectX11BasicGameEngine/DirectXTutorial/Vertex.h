#pragma once

struct Vertex
{
	Vertex() : Pos(0.0f, 0.0f, 0.0f), Normal(0.0f, 0.0f, 0.0f), Tex0(0.0f, 0.0f) {}
	Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT2& uv)
		: Pos(p), Normal(n), Tex0(uv) {}
	Vertex(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
		: Pos(px, py, pz), Normal(nx, ny, nz), Tex0(u, v) {}
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex0;
};