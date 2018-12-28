#include "stdafx.h"
#include "Material.h"

Material::Material()
	: shader(NULL)
	, diffuseMap(NULL), specularMap(NULL), normalMap(NULL), detailMap(NULL)
	, bShaderDelete(false)
{
	buffer = new Buffer();
}

Material::Material(wstring shaderFile)
	: shader(NULL), diffuseMap(NULL), specularMap(NULL), normalMap(NULL), detailMap(NULL)
	, bShaderDelete(true)
{
	assert(shaderFile.length() > 0);

	buffer = new Buffer();
	shader = new Shader(shaderFile);
}

Material::~Material()
{
	if (bShaderDelete)
		SAFE_DELETE(shader);

	SAFE_DELETE(detailMap);
	SAFE_DELETE(normalMap);
	SAFE_DELETE(diffuseMap);
	SAFE_DELETE(specularMap);
	SAFE_DELETE(buffer);
}

void Material::Clone(void ** clone)
{
	Material* material = new Material();

	material->Name(this->name);

	if(this->shader != NULL)
		material->SetShader(this->shader->GetFile());

	material->SetDiffuse(*this->GetDiffuse());
	material->SetSpecular(*this->GetSpecular());

	if (this->diffuseMap != NULL)
		material->SetDiffuseMap(this->diffuseMap->GetFile());

	if (this->specularMap != NULL)
		material->SetSpecularMap(this->specularMap->GetFile());

	if (this->normalMap != NULL)
		material->SetNormalMap(this->normalMap->GetFile());

	material->SetShininess(*this->GetShininess());

	*clone = material;
}

void Material::SetShader(string file, string vsName, string psName)
{
	SetShader(String::ToWString(file), vsName, psName);
}

void Material::SetShader(wstring file, string vsName, string psName)
{
	if (bShaderDelete == true)
		SAFE_DELETE(shader);

	bShaderDelete = false;

	if (file.length() > 0) {
		shader = new Shader(file, vsName, psName);
		bShaderDelete = true;
	}
}

void Material::SetShader(Shader * shader)
{
	if (bShaderDelete)
		SAFE_DELETE(this->shader);

	this->shader = shader;
	bShaderDelete = false;
}

void Material::SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDiffuseMap(String::ToWString(file), info);
}

void Material::SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(diffuseMap);
	
	diffuseMap = new Texture(file, info);
}

void Material::SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetSpecularMap(String::ToWString(file), info);
}

void Material::SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(specularMap);

	specularMap = new Texture(file, info);
}

void Material::SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetNormalMap(String::ToWString(file), info);
}

void Material::SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(normalMap);

	normalMap = new Texture(file, info);
}

void Material::SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDetailMap(String::ToWString(file), info);
}

void Material::SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(detailMap);

	detailMap = new Texture(file, info);
}

void Material::PSSetBuffer()
{
	if (shader != NULL)
		shader->Render();

	UINT slot = 0;
	if (diffuseMap != NULL)
	{
		diffuseMap->SetShaderResource(slot);
		diffuseMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}

	slot = 1;
	if (specularMap != NULL)
	{
		specularMap->SetShaderResource(slot);
		specularMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}

	slot = 2;
	if (normalMap != NULL)
	{
		normalMap->SetShaderResource(slot);
		normalMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}

	slot = 3;
	if (detailMap != NULL)
	{
		detailMap->SetShaderResource(slot);
		detailMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}

	buffer->SetPSBuffer(1);
}
