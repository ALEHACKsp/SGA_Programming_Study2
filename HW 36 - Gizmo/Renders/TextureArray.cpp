#include "stdafx.h"
#include "TextureArray.h"

using namespace DirectX;

TextureArray::TextureArray(vector<wstring> & names, UINT width, UINT height, UINT mipLevels)
{
	vector<ID3D11Texture2D *> textures;
	textures = CreateTextures(names, width, height, mipLevels);


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
		desc.Format = textureDesc.Format;;
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
				D3D::GetDC()->UpdateSubresource(textureArray, D3D11CalcSubresource(level, i, textureDesc.MipLevels), NULL, subResource.pData, subResource.RowPitch, subResource.DepthPitch);
			}
			D3D::GetDC()->Unmap(textures[i], level);
		}
	}

	//   Create File textures[0] -> test.png (for Test)
	//D3DX11SaveTextureToFile(D3D::GetDC(), textureArray, D3DX11_IFF_PNG, L"test.png");

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

vector<ID3D11Texture2D*> TextureArray::CreateTextures(vector<wstring>& names, UINT width, UINT height, UINT mipLevels)
{
	vector<ID3D11Texture2D *> returnTextures;
	returnTextures.resize(names.size());

	for (UINT index = 0; index < returnTextures.size(); index++)
	{
		HRESULT hr;

		TexMetadata metaData;
		wstring ext = Path::GetExtension(names[index]);
		if (ext == L"tga")
		{
			hr = GetMetadataFromTGAFile(names[index].c_str(), metaData);
			assert(SUCCEEDED(hr));
		}
		else if (ext == L"dds")
		{
			hr = GetMetadataFromDDSFile(names[index].c_str(), DDS_FLAGS_NONE, metaData);
			assert(SUCCEEDED(hr));
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = GetMetadataFromWICFile(names[index].c_str(), WIC_FLAGS_NONE, metaData);
			assert(SUCCEEDED(hr));
		}

		ScratchImage image;

		if (ext == L"tga")
		{
			hr = LoadFromTGAFile(names[index].c_str(), &metaData, image);
			assert(SUCCEEDED(hr));
		}
		else if (ext == L"dds")
		{
			hr = LoadFromDDSFile(names[index].c_str(), DDS_FLAGS_NONE, &metaData, image);
			assert(SUCCEEDED(hr));
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = LoadFromWICFile(names[index].c_str(), WIC_FLAGS_FORCE_RGB | WIC_FLAGS_IGNORE_SRGB, &metaData, image);
			assert(SUCCEEDED(hr));
		}

		ScratchImage resizedImage;
		hr = DirectX::Resize
		(
			image.GetImages(), image.GetImageCount(), image.GetMetadata(), width, height, TEX_FILTER_DEFAULT, resizedImage
		);
		assert(SUCCEEDED(hr));

		if (mipLevels > 1)
		{
			ScratchImage mipmapedImage;
			hr = DirectX::GenerateMipMaps
			(
				resizedImage.GetImages(), resizedImage.GetImageCount(), resizedImage.GetMetadata(), D3DX11_FILTER_NONE, mipLevels, mipmapedImage
			);
			assert(SUCCEEDED(hr));

			hr = DirectX::CreateTextureEx
			(
				D3D::GetDevice()
				, mipmapedImage.GetImages()
				, mipmapedImage.GetImageCount()
				, mipmapedImage.GetMetadata()
				, D3D11_USAGE_STAGING
				, 0
				, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
				, 0
				, false
				, (ID3D11Resource **)&returnTextures[index]
			);
			assert(SUCCEEDED(hr));

			mipmapedImage.Release();
		}
		else
		{
			hr = DirectX::CreateTextureEx
			(
				D3D::GetDevice()
				, resizedImage.GetImages()
				, resizedImage.GetImageCount()
				, resizedImage.GetMetadata()
				, D3D11_USAGE_STAGING
				, 0
				, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
				, 0
				, false
				, (ID3D11Resource **)&returnTextures[index]
			);
			assert(SUCCEEDED(hr));
		}

		image.Release();
		resizedImage.Release();

	}
	return returnTextures;
}