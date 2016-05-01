#include "Geometry.h"

using namespace std;

void Geometry::BuildGeometryObject(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB)
{
	GeometryGenerator::MeshData box;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	BoxVertexOffset = 0;
	mBoxIndexCount = box.Indices.size();
	mBoxIndexOffset = 0;

	vector<Vertex> vertices(box.Indices.size());

	XMFLOAT4 blue(0.0f, 0.0f, 1.0f, 1.0f);

	UINT k = 0;


	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex0 = box.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;												//정점 버퍼 생성
	vbd.Usage = D3D11_USAGE_IMMUTABLE;									//버퍼가 쓰이는 방식
	vbd.ByteWidth = sizeof(Vertex) * box.Vertices.size();				//생성할 정점 버퍼의 크기
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;												//CPU가 버퍼에 접근하는 방식을 결정하는 플래그 지정
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];									//정점 버퍼를 초기화할 자료를 담은 포인터

	HR(device->CreateBuffer(&vbd, &vinitData, mVB));

	D3D11_BUFFER_DESC ibd; //색인 버퍼
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mBoxIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	//색인버퍼 초기화 자료 지정
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &box.Indices[0];

	//색인버퍼 생성
	HR(device->CreateBuffer(&ibd, &iinitData, mIB));
}

float Geometry::GetHeight(float x, float z)const
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

XMFLOAT3 Geometry::GetHillNormal(float x, float z)const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void Geometry::BuildGeometryLand(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB)
{
	//언덕
	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;

	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	mGridIndexCount = grid.Indices.size();

	std::vector<Vertex> vertices(grid.Vertices.size());

	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = XMFLOAT3(0.0f, 0.0f, 0.0f);

		p = grid.Vertices[i].Position;

		p.y = GetHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = GetHillNormal(p.x, p.z);
		vertices[i].Tex0 = grid.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;						//정점 버퍼 생성
	vbd.Usage = D3D11_USAGE_IMMUTABLE;			//버퍼가 쓰이는 방식 
	vbd.ByteWidth = sizeof(Vertex) * grid.Vertices.size();			//생성할 정점 버퍼의 크기
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;						//CPU가 버퍼에 접근하는 방식을 결정하는 플래그 지정
	vbd.MiscFlags = 0;
	//vbd.StructureByteStride = 0;				// 구조적 버퍼에 저장된 원소 하나의 크기
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];				//정점 버퍼를 초기화할 자료를 담은 포인터

	HR(device->CreateBuffer(
		&vbd,			//생성할 버퍼를 서술하는 구조체
		&vinitData,		//버퍼를 초기화하는데 사용할 자료
		mVB));		//생성된 버퍼가 설정되는 곳

	D3D11_BUFFER_DESC ibd; //색인 버퍼
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	//ibd.StructureByteStride = 0;

	//색인버퍼 초기화 자료 지정
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];

	//색인버퍼 생성
	HR(device->CreateBuffer(&ibd, &iinitData, mIB));
}

void Geometry::BuildGeometryWave(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB)
{
	mWaves.Init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);

	std::vector<Vertex> vertices(mWaves.VertexCount());

	for (size_t i = 0; i < mWaves.VertexCount(); ++i)
	{
		vertices[i].Tex0.x = 0.5f + mWaves[i].x / mWaves.Width();
		vertices[i].Tex0.y = 0.5f - mWaves[i].z / mWaves.Depth();
	}

	//파도
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * mWaves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(device->CreateBuffer(&vbd, 0, mVB));

	std::vector<UINT> indices(3 * mWaves.TriangleCount()); // 3 indices per face
														   // Iterate over each quad.
	UINT m = mWaves.RowCount();
	UINT n = mWaves.ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i*n + j;
			indices[k + 1] = i*n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i*n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, mIB));
}

void Geometry::BuildWallObject(ID3D11Device* device, ID3D11Buffer** mVB)
{
	Vertex v[24];

	v[0] = Vertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[1] = Vertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);

	v[3] = Vertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[4] = Vertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);
	v[5] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f);

	v[6] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[7] = Vertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);

	v[9] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
	v[10] = Vertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
	v[11] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f);

	v[12] = Vertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);

	v[15] = Vertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);
	v[17] = Vertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f);

	// Mirror
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 24;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = v;
	HR(device->CreateBuffer(&vbd, &vinitData, mVB));
}

void Geometry::UpdateGeometryWave(ID3D11DeviceContext* context, float TotalTime, ID3D11Buffer** mVB, ID3D11Buffer** mIB, float dt)
{
	static float t_base = 0.0f;
	if ((TotalTime - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % (mWaves.RowCount() - 10);
		DWORD j = 5 + rand() % (mWaves.ColumnCount() - 10);

		float r = MathHelper::RandF(1.0f, 2.0f);

		mWaves.Disturb(i, j, r);
	}

	mWaves.Update(dt);

	//
	// Update the wave vertex buffer with the new solution.
	//

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(context->Map(*mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Normal = mWaves.Normal(i);
	}

	context->Unmap(*mVB, 0);
}

UINT Geometry::GetBoxIndexCount()
{
	return mBoxIndexCount;
}

UINT Geometry::GetGridIndexCount()
{
	return mGridIndexCount;
}

UINT Geometry::GetWaveTriangleCount()
{
	return mWaves.TriangleCount();
}
