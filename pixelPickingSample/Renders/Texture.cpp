#include "stdafx.h"
#include "Texture.h"

using namespace DirectX;

vector<TextureDesc> Textures::descs;

Texture::Texture(wstring file, D3DX11_IMAGE_LOAD_INFO* loadInfo)
	: file(file)
{
	Textures::Load(this, loadInfo);

	samplerState = new SamplerState();
}

Texture::~Texture()
{
	SAFE_DELETE(samplerState);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixels(DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels)
{
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	return ReadPixels(srcTexture, readFormat, pixels);
}

void Texture::WritePixels(DXGI_FORMAT writeFormat, vector<D3DXCOLOR>& pixels, bool isInteger)
{
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	D3D11_TEXTURE2D_DESC writeTexturedesc;
	ZeroMemory(&writeTexturedesc, sizeof(D3D11_TEXTURE2D_DESC));
	writeTexturedesc.Width = metaData.width;
	writeTexturedesc.Height = metaData.height;
	writeTexturedesc.MipLevels = metaData.mipLevels;
	writeTexturedesc.ArraySize = metaData.arraySize;
	writeTexturedesc.Format = writeFormat;
	writeTexturedesc.SampleDesc.Count = 1;
	writeTexturedesc.Usage = D3D11_USAGE_STAGING;
	writeTexturedesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	writeTexturedesc.MiscFlags = metaData.miscFlags;

	HRESULT hr;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&writeTexturedesc, NULL, &texture);
	assert(SUCCEEDED(hr));

	D3D11_MAPPED_SUBRESOURCE map;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_WRITE, NULL, &map);
	{
		for (UINT y = 0; y < writeTexturedesc.Height; y++)
		{
			for (UINT x = 0; x < writeTexturedesc.Width; x++)
			{
				UINT index = writeTexturedesc.Width * y + x;

				UINT val = 0;
				if (isInteger)
				{
					UINT r = ((UINT)(pixels[index].r) & 0x000000FF) << 0;
					UINT g = ((UINT)(pixels[index].g) & 0x000000FF) << 8;
					UINT b = ((UINT)(pixels[index].b) & 0x000000FF) << 16;
					UINT a = ((UINT)(pixels[index].a) & 0x000000FF) << 24;
					*((UINT*)(map.pData) + index) = r + g + b + a;
				}
				else
				{
					UINT r = ((UINT)(255 * pixels[index].r) & 0x000000FF) << 0;
					UINT g = ((UINT)(255 * pixels[index].g) & 0x000000FF) << 8;
					UINT b = ((UINT)(255 * pixels[index].b) & 0x000000FF) << 16;
					UINT a = ((UINT)(255 * pixels[index].a) & 0x000000FF) << 24;
					*((UINT*)(map.pData) + index) = r + g + b + a;
				}
			}
		}
	}
	D3D::GetDC()->Unmap(texture, 0);

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), texture, NULL, srcTexture);
	assert(SUCCEEDED(hr));
}

void Texture::SaveFile(wstring file)
{
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	SaveFile(file, srcTexture);
}

void Texture::SetSamplerState(SamplerState* state)
{
	SAFE_DELETE(samplerState);

	samplerState = state;
}

void Texture::SetShaderResource(UINT slot)
{
	D3D::GetDC()->PSSetShaderResources(slot, 1, &view);
}

void Texture::SetShaderSampler(UINT slot)
{
	samplerState->PSSetSamplers(slot);
}

void Texture::SetShaderResources(UINT slot, UINT count, Texture ** textures)
{
	vector<ID3D11ShaderResourceView *> views;
	for (UINT i = 0; i < count; i++)
		views.push_back(textures[i]->GetView());

	D3D::GetDC()->PSSetShaderResources(slot, count, &views[0]);
}

void Texture::SetBlankShaderResource(UINT slot)
{
	ID3D11ShaderResourceView* null[1]{ NULL };

	D3D::GetDC()->PSSetShaderResources(slot, 1, null);
}

void Texture::SetBlankSamplerState(UINT slot)
{
	ID3D11SamplerState* null[1]{ NULL };

	D3D::GetDC()->PSSetSamplers(slot, 1, null);
}

void Texture::SaveFile(wstring file, ID3D11Texture2D * src)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);

	ID3D11Texture2D* dest;
	D3D11_TEXTURE2D_DESC destDesc;
	ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
	destDesc.Width = srcDesc.Width;
	destDesc.Height = srcDesc.Height;
	destDesc.MipLevels = 1;
	destDesc.ArraySize = 1;
	destDesc.Format = srcDesc.Format;
	destDesc.SampleDesc = srcDesc.SampleDesc;
	destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	destDesc.Usage = D3D11_USAGE_STAGING;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&destDesc, NULL, &dest);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), src, NULL, dest);
	assert(SUCCEEDED(hr));

	hr = D3DX11SaveTextureToFile(D3D::GetDC(), dest, D3DX11_IFF_PNG, file.c_str());
	assert(SUCCEEDED(hr));

	SAFE_RELEASE(dest);
}

UINT Texture::ReadPixel32Bit(ID3D11Texture2D * src, UINT xPos, UINT yPos)
{
	D3D11_BOX srcBox;
	srcBox.left = xPos;
	srcBox.right = srcBox.left + 1;
	srcBox.top = yPos;
	srcBox.bottom = srcBox.top + 1;
	srcBox.front = 0;
	srcBox.back = 1;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	HRESULT hr;

	ID3D11Texture2D* p1x1StagingTexture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &p1x1StagingTexture);
	assert(SUCCEEDED(hr));

	D3D::GetDC()->CopySubresourceRegion(p1x1StagingTexture, 0, 0, 0, 0, src, 0, &srcBox);

	D3D11_MAPPED_SUBRESOURCE map;
	
	UINT pixel;
	D3D::GetDC()->Map(p1x1StagingTexture, 0, D3D11_MAP_READ, 0, &map);
	memcpy(&pixel, map.pData, sizeof(UINT));
	D3D::GetDC()->Unmap(p1x1StagingTexture, 0);

	return pixel;
}

D3D11_TEXTURE2D_DESC Texture::ReadPixels(ID3D11Texture2D * src, DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = srcDesc.Width;
	desc.Height = srcDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = readFormat;
	desc.SampleDesc = srcDesc.SampleDesc;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;


	HRESULT hr;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), src, NULL, texture);
	assert(SUCCEEDED(hr));


	D3D11_MAPPED_SUBRESOURCE map;
	UINT* colors = new UINT[desc.Width * desc.Height];
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, NULL, &map);
	{
		memcpy(colors, map.pData, sizeof(UINT) * desc.Width * desc.Height);
	}
	D3D::GetDC()->Unmap(texture, 0);


	pixels->reserve(desc.Width * desc.Height);
	for (UINT y = 0; y < desc.Height; y++)
	{
		for (UINT x = 0; x < desc.Width; x++)
		{
			UINT index = desc.Width * y + x;

			CONST FLOAT f = 1.0f / 255.0f;
			float r = f * (float)((0xFF000000 & colors[index]) >> 24);
			float g = f * (float)((0x00FF0000 & colors[index]) >> 16);
			float b = f * (float)((0x0000FF00 & colors[index]) >> 8);
			float a = f * (float)((0x000000FF & colors[index]) >> 0);

			pixels->push_back(D3DXCOLOR(a, b, g, r));
		}
	}

	SAFE_DELETE_ARRAY(colors);
	SAFE_RELEASE(src);
	SAFE_RELEASE(texture);

	return desc;
}

void Texture::WritePixels(wstring& fileName, VertexTextureNormalTangentSplatting* vertices, UINT& height, UINT& width)
{
	ID3D11Texture2D *pTex = NULL;
	HRESULT hr;

	D3D11_TEXTURE2D_DESC destDesc;
	ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
	destDesc.Width = ++height;
	destDesc.Height = ++width;
	destDesc.MipLevels = destDesc.ArraySize = 1;
	destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	destDesc.SampleDesc.Count = 1;
	destDesc.Usage = D3D11_USAGE_STAGING;
	destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	destDesc.MiscFlags = 0;
	hr = D3D::GetDevice()->CreateTexture2D(&destDesc, NULL, &pTex);

	D3D11_MAPPED_SUBRESOURCE  mapResource;
	D3D::GetDC()->Map(pTex, 0, D3D11_MAP_WRITE, NULL, &mapResource);
	{
		vector<UINT> pixels;
		for (UINT y = 0; y < destDesc.Height; y++)
		{
			for (UINT x = 0; x < destDesc.Width; x++)
			{
				UINT index = destDesc.Width * y + x;
				UINT val = 0;
				val += (UINT)(vertices[index].Splatting.x * 255.0f) << 24;
				val += (UINT)(vertices[index].Splatting.y * 255.0f) << 16;
				val += (UINT)(vertices[index].Splatting.z * 255.0f) << 8;
				val += (UINT)(vertices[index].Splatting.w * 255.0f);

				pixels.push_back(val);
			}
		}
		memcpy(mapResource.pData, &pixels[0], sizeof(UINT)*(destDesc.Width * destDesc.Height));
	}
	D3D::GetDC()->Unmap(pTex, 0);

	hr = D3DX11SaveTextureToFile(D3D::GetDC(), pTex, D3DX11_IFF_PNG, fileName.c_str());
	assert(SUCCEEDED(hr));
	SAFE_RELEASE(pTex);
}

void Textures::Create()
{

}

void Textures::Delete()
{
	for (TextureDesc desc : descs)
		SAFE_RELEASE(desc.view);
}

void Textures::Load(Texture * texture, D3DX11_IMAGE_LOAD_INFO * loadInfo)
{
	HRESULT hr;

	TexMetadata metaData;
	wstring ext = Path::GetExtension(texture->file);
	if (ext == L"tga")
	{
		hr = GetMetadataFromTGAFile(texture->file.c_str(), metaData);
		assert(SUCCEEDED(hr));
	}
	else if (ext == L"dds")
	{
		hr = GetMetadataFromDDSFile(texture->file.c_str(), DDS_FLAGS_NONE, metaData);
		assert(SUCCEEDED(hr));
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else
	{
		hr = GetMetadataFromWICFile(texture->file.c_str(), WIC_FLAGS_NONE, metaData);
		assert(SUCCEEDED(hr));
	}

	UINT width = metaData.width;
	UINT height = metaData.height;

	if (loadInfo != NULL)
	{
		width = loadInfo->Width;
		height = loadInfo->Height;

		metaData.width = loadInfo->Width;
		metaData.height = loadInfo->Height;
	}


	TextureDesc desc;
	desc.file = texture->file;
	desc.width = width;
	desc.height = height;

	TextureDesc exist;
	bool bExist = false;
	for (TextureDesc temp : descs)
	{
		if (desc == temp)
		{
			bExist = true;
			exist = temp;

			break;
		}
	}

	if (bExist == true)
	{
		texture->metaData = exist.metaData;
		texture->view = exist.view;
	}
	else
	{
		ScratchImage image;
		if (ext == L"tga")
		{
			hr = LoadFromTGAFile(texture->file.c_str(), &metaData, image);
			assert(SUCCEEDED(hr));
		}
		else if (ext == L"dds")
		{
			hr = LoadFromDDSFile(texture->file.c_str(), DDS_FLAGS_NONE, &metaData, image);
			assert(SUCCEEDED(hr));
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = LoadFromWICFile(texture->file.c_str(), WIC_FLAGS_NONE, &metaData, image);
			assert(SUCCEEDED(hr));
		}

		ID3D11ShaderResourceView* view;

		hr = DirectX::CreateShaderResourceView(D3D::GetDevice(), image.GetImages(), image.GetImageCount(), metaData, &view);
		assert(SUCCEEDED(hr));

		desc.file = texture->file;
		desc.width = metaData.width;
		desc.height = metaData.height;
		desc.view = view;

		texture->view = view;
		texture->metaData = metaData;

		descs.push_back(desc);
	}
}

TextureArray::TextureArray(vector<wstring> & names, UINT width, UINT height)
{
	D3DX11_IMAGE_LOAD_INFO info;
	ZeroMemory(&info, sizeof(D3DX11_IMAGE_LOAD_INFO));

	info.Width = width;
	info.Height = height;
	info.Depth = 0;
	info.FirstMipLevel = 0;
	info.MipLevels = 0;
	info.Usage = D3D11_USAGE_STAGING;
	info.BindFlags = 0;
	info.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	info.MiscFlags = 0;
	info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	info.Filter = D3DX11_FILTER_NONE;
	info.MipFilter = D3DX11_FILTER_NONE;
	info.pSrcInfo = NULL;


	vector<ID3D11Texture2D *> textures;
	textures.resize(names.size());
	for (UINT i = 0; i < names.size(); i++)
	{
		HRESULT hr = D3DX11CreateTextureFromFile(D3D::GetDevice(), names[i].c_str(), &info, NULL, (ID3D11Resource **)&textures[i], NULL);
		assert(SUCCEEDED(hr));
	}


	D3D11_TEXTURE2D_DESC textureDesc;
	textures[0]->GetDesc(&textureDesc);

	ID3D11Texture2D* textureArray;
	//Texture2DArray 持失
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = textureDesc.Width;
		desc.Height = textureDesc.Height;
		desc.MipLevels = textureDesc.MipLevels;
		desc.ArraySize = names.size();
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &textureArray);
		assert(SUCCEEDED(hr));
	}


	for (UINT i = 0; i < textures.size(); i++)
	{
		for (UINT level = 0; level < textureDesc.MipLevels; level++)
		{
			D3D11_MAPPED_SUBRESOURCE subResource;
			D3D::GetDC()->Map(textures[i], level, D3D11_MAP_READ, 0, &subResource);
			{
				D3D::GetDC()->UpdateSubresource(textureArray, D3D11CalcSubresource(level, i, textureDesc.MipLevels), NULL, subResource.pData, subResource.RowPitch, 0);
			}
			D3D::GetDC()->Unmap(textures[i], level);
		}
	}

	D3DX11SaveTextureToFile(D3D::GetDC(), textureArray, D3DX11_IFF_PNG, L"test.png");


	//SRV 持失
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = textureDesc.Format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MostDetailedMip = 0;
		desc.Texture2DArray.MipLevels = textureDesc.MipLevels;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = names.size();

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(textureArray, &desc, &srv);
		assert(SUCCEEDED(hr));
	}

	for (ID3D11Texture2D* texture : textures)
		SAFE_RELEASE(texture);

	SAFE_RELEASE(textureArray);
}

TextureArray::~TextureArray()
{
	SAFE_RELEASE(srv);
}