#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#define BUF_SIZE 1024
struct vec3
{
	float x, y, z;
	vec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
		:x(_x), y(_y), z(_z)	{}
	friend std::ostream& operator<<(std::ostream& out, const vec3& vert);
};
std::ostream& operator<<(std::ostream& out, const vec3& vert)
{
	char buffer[BUF_SIZE];
	snprintf(buffer, BUF_SIZE, "%.6f %.6f %.6f", vert.x, vert.y, vert.z);
	out << buffer;
	return out;
}
struct vec2
{
	float x, y;
	vec2(float _x = 0.0f, float _y = 0.0f)
		:x(_x), y(_y) {}
	friend std::ostream& operator<<(std::ostream& out, const vec2& vert);
};
std::ostream& operator<<(std::ostream& out, const vec2& vert)
{
	char buffer[BUF_SIZE];
	snprintf(buffer, BUF_SIZE, "%.6f %.6f", vert.x, vert.y);
	out << buffer;
	return out;
}
struct Index
{
	int v, vt, vn;
	Index(int _v = 0, int _vt = 0, int _vn = 0)
		:v(_v), vt(_vt), vn(_vn)	{}
	friend bool operator<(const Index& left, const Index& right);
};

bool operator<(const Index& left, const Index& right)
{
	unsigned long long lval = std::stoull(std::to_string(left.v) + std::to_string(left.vt) + std::to_string(left.vn));
	unsigned long long rval = std::stoull(std::to_string(right.v) + std::to_string(right.vt) + std::to_string(right.vn));
	return lval < rval;
}
struct Vertex
{
	vec3 position;
	vec2 texCoord;
	vec3 normal;
	Vertex(vec3 pos, vec2 tex, vec3 norm)
		:position(pos), texCoord(tex), normal(norm)		{}
};
struct Material_ptr // assigns a block of indicies to a material
{
	unsigned int m_index;
	std::string m_mtlName;
	Material_ptr(unsigned int index, std::string mtlName = "")
		:m_index(index), m_mtlName(mtlName)
	{

	}
};
// Forward declarations
inline std::string calc_root_dir(std::string filepath);
bool buffer_obj(const std::string& filepath,
	std::vector<vec3>& vPos,
	std::vector<vec3>& vNorm,
	std::vector<vec2>& vTex,
	std::vector<Index>& iBuf,
	std::vector<std::string>& mtlPaths,
	std::vector<Material_ptr>& mat_ptrs);
bool expand_obj(std::vector<vec3>& vPos,
	std::vector<vec3>& vNorm,
	std::vector<vec2>& vTex,
	std::vector<Index>& iBuf,
	std::vector<Vertex>& vertexBuffer,
	std::vector<unsigned int>& indexBuffer);
bool write_to_file(const std::string& filename,
	const std::vector<Vertex>& vertexBuffer,
	const std::vector<unsigned>& indexBuffer,
	std::vector<std::string>& mtlPaths,
	std::vector<Material_ptr>& mat_ptrs);
bool generate_sphere(std::vector<Vertex>& vertexBuffer, std::vector<unsigned>& indexBuffer);
void generate_cylinder(std::vector<Vertex>& vertexBuffer, std::vector<unsigned>& indexBuffer);
int main()
{
	std::string inputfile, outputfile;
	std::cout << "Enter Input file:\n";
	std::cin >> inputfile;

	std::chrono::steady_clock::time_point a = std::chrono::steady_clock::now();
	std::vector<vec3> vPos;
	std::vector<vec3> vNorm;
	std::vector<vec2> vTex;
	std::vector<Index> iBuf;
	std::vector<std::string> mtlPaths;
	std::vector<Material_ptr> mat_ptrs;

	buffer_obj(inputfile, vPos, vNorm, vTex, iBuf, mtlPaths, mat_ptrs);

	std::vector<Vertex> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	//generate_sphere(vertexBuffer, indexBuffer);
	//generate_cylinder(vertexBuffer, indexBuffer);
	expand_obj(vPos, vNorm, vTex, iBuf, vertexBuffer, indexBuffer);
	write_to_file(inputfile + ".expanded", vertexBuffer, indexBuffer, mtlPaths, mat_ptrs);

	std::chrono::steady_clock::time_point b = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> time = b - a;
	std::cout << time.count() << "ms\n";
	return 0;
}

bool buffer_obj(const std::string& filepath,
	std::vector<vec3>& vPos,
	std::vector<vec3>& vNorm,
	std::vector<vec2>& vTex,
	std::vector<Index>& iBuf,
	std::vector<std::string>& mtlPaths,
	std::vector<Material_ptr>& mat_ptrs)
	// TODO: ERROR HANDLING
{
	std::ifstream file(filepath);
	std::string line;

	unsigned int fIndex = 0; // points to 1 off the last index
	while (std::getline(file, line))
	{
		std::stringstream s(line);
		std::string id;
		s >> id;
		if (id == "v")
		{
			float x, y, z;
			s >> x; s >> y; s >> z;
			vPos.emplace_back(x, y, z);
		}
		else if (id == "vn")
		{
			float x, y, z;
			s >> x; s >> y; s >> z;
			vNorm.emplace_back(x, y, z);
		}
		else if (id == "vt")
		{
			float x, y;
			s >> x; s >> y;
			vTex.emplace_back(x, y);
		}
		else if (id == "f")
		{
			std::string data;
			int iterations = 0;
			while (s >> data)
			{
				int v, vt, vn;
				std::sscanf(data.c_str(), "%i/%i/%i", &v, &vt, &vn);
				iBuf.emplace_back(v - 1, vt - 1, vn - 1);
				iterations++;
			}
			if (iterations == 4) // Rectangle was specified, assume points are specifed in ACW winding order
			{
				int begin = iBuf.size() - 4;
				Index i1 = iBuf[begin];
				Index i2 = iBuf[begin + 1];
				Index i3 = iBuf[begin + 2];
				Index i4 = iBuf[begin + 3];
				// reorder quad into 2 triangles
				iBuf[begin + 3] = i1;
				iBuf.push_back(i3);
				iBuf.push_back(i4);
				fIndex += 3; // extra triangle was added
			}
			if (mat_ptrs.size() == 0) // if no material has been declared
			{
				mat_ptrs.emplace_back(0, "ERR_NO_MTL");
			}
			fIndex += 3;
		}
		else if (id == "usemtl")
		{
			s >> id;
			mat_ptrs.emplace_back(fIndex, id);
		}
		else if (id == "mtllib")
		{
			// we assume filepath is relative to directory containing .obj file
			s >> id;
			mtlPaths.push_back(id);
		}
	}
	return true;
}
bool expand_obj(std::vector<vec3>& vPos,
	std::vector<vec3>& vNorm,
	std::vector<vec2>& vTex,
	std::vector<Index>& iBuf,
	std::vector<Vertex>& vertexBuffer,
	std::vector<unsigned int>& indexBuffer)
	// Duplicates vertices
	// TODO: ERROR HANDLING
{
	std::map<Index, unsigned int> map;
	unsigned int last_index = 0;
	for (auto index : iBuf)
	{
		if (map.find(index) == map.end())	// if no index exsists
		{
			// convert negative indices to positive
			unsigned int vIndex = index.v >= 0 ? index.v : vPos.size() + index.v + 1;
			unsigned int vtIndex = index.vt >= 0 ? index.vt : vTex.size() + index.vt + 1;
			unsigned int vnIndex = index.vn >= 0 ? index.vn : vNorm.size() + index.vn + 1;
			vertexBuffer.emplace_back(vPos[vIndex], vTex[vtIndex], vNorm[vnIndex]);
			indexBuffer.emplace_back(last_index + 1);
			map.insert(std::make_pair(index, last_index));
			++last_index;
		}
		else
			indexBuffer.emplace_back(map[index] + 1);
	}
	return true;
}
bool write_to_file(const std::string& filename,
	const std::vector<Vertex>& vertexBuffer,
	const std::vector<unsigned>& indexBuffer,
	std::vector<std::string>& mtlPaths,
	std::vector<Material_ptr>& mat_ptrs)
{
	std::ofstream output(filename);

	for (const auto filepath : mtlPaths)
		output << "mtllib " << filepath << '\n';

	for (const auto vert : vertexBuffer)
		output << "v " << vert.position << '\n';
	for (const auto vert : vertexBuffer)
		output << "vt " << vert.texCoord << '\n';
	for (const auto vert : vertexBuffer)
		output << "vn " << vert.normal << '\n';
	
	output << '\n';
	for (int i = 0; i < indexBuffer.size(); i += 3)
	{
		for (const auto mtl : mat_ptrs)
			if (i == mtl.m_index)
				output << "usemtl " << mtl.m_mtlName << '\n';
		
		output << "f " << indexBuffer[i] << '/' << indexBuffer[i] << '/' << indexBuffer[i];
		output << ' ' << indexBuffer[i + 1] << '/' << indexBuffer[i + 1] << '/' << indexBuffer[i + 1];
		output << ' ' << indexBuffer[i + 2] << '/' << indexBuffer[i + 2] << '/' << indexBuffer[i + 2];
		output << '\n';
	}
	return true;
}
bool generate_sphere(std::vector<Vertex>& vertexBuffer, std::vector<unsigned>& indexBuffer)
{
	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	const float PI = 3.14159265359;
	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			vec3 pos(xPos, yPos, zPos);
			vec2 UV(xSegment, ySegment);
			vertexBuffer.emplace_back(pos, UV, pos);
		}
	}
	bool oddRow = false;
	for (int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (int x = 0; x <= X_SEGMENTS; ++x)
			{
				indexBuffer.push_back(y * (X_SEGMENTS + 1) + x);
				indexBuffer.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indexBuffer.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indexBuffer.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	return true;
}
void generate_cylinder(std::vector<Vertex>& vertexBuffer, std::vector<unsigned>& indexBuffer)
{
	const int NR_TEXTURE_WRAPS = 3;
	const float PI = 3.14159265359;
	float height = 1.0f;
	float radius = NR_TEXTURE_WRAPS / (2.0f * PI); // circumference is NR_TEXTURE_WRAPS;
	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;

	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x < X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)(X_SEGMENTS - 1);
			float ySegment = (float)y / (float)(Y_SEGMENTS - 1);

			float xPos = radius * cosf(xSegment * 2.0f * PI);
			float yPos = (ySegment - 0.5f) * height;
			float zPos = radius * sinf(xSegment * 2.0f * PI);

			vec3 Pos(xPos, yPos, zPos);
			vec3 Norm(xPos, 0.0f, zPos);
			vec2 UV(xSegment * NR_TEXTURE_WRAPS, ySegment);

			vertexBuffer.emplace_back(Pos, UV, Norm);
		}
	}
	auto index = [X_SEGMENTS](int x, int y)->unsigned int { return (y - 1) * X_SEGMENTS + x; };
	for (unsigned int y = 1; y < Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 1; x < X_SEGMENTS; ++x)
		{
			indexBuffer.push_back(index(x, y));
			indexBuffer.push_back(index(x + 1, y + 1));
			indexBuffer.push_back(index(x, y + 1));

			indexBuffer.push_back(index(x, y));
			indexBuffer.push_back(index(x + 1, y));
			indexBuffer.push_back(index(x + 1, y + 1));
		}

	}
}
inline std::string calc_root_dir(std::string filepath)
// given "res/Objects/Pokemon/Pikachu.obj" returns "res/Objects/Pokemon/"
{
	unsigned int index = filepath.rfind('/');
	if (index < filepath.size())
		return filepath.substr(0, index + 1U);
	else return "";
}