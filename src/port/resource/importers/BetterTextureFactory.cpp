#include "BetterTextureFactory.h"
#include "fast/resource/type/Texture.h"
#include "spdlog/spdlog.h"
#include <stb_image.h>
#include <ship/Context.h>
#include "ship/resource/archive/ArchiveManager.h"
#include "ship/resource/ResourceManager.h"

namespace MK64 {

std::shared_ptr<Ship::IResource> loadPngTexture(std::shared_ptr<Ship::File> filePng, std::shared_ptr<Ship::ResourceInitData> initData) {
    auto texture = std::make_shared<Fast::Texture>(initData);

    int height, width = 0;
    texture->ImageData = stbi_load_from_memory((const stbi_uc*)filePng->Buffer.get()->data(),
                                               filePng->Buffer.get()->size(), &width, &height, nullptr, 4);
    texture->Width = width;
    texture->Height = height;
    texture->Type = Fast::TextureType::RGBA32bpp;
    texture->ImageDataSize = texture->Width * texture->Height * 4;
    texture->Flags = TEX_FLAG_LOAD_AS_IMG;
    return texture;
}

std::vector<std::string> extension = {".png", ".PNG", ".jpg", ".JPG", ".jpeg", ".JPEG", ".bmp", ".BMP"};

std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryTextureV0::ReadResource(std::shared_ptr<Ship::File> file,
                                             std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    for (const auto& ext : extension) {
        auto filePng = Ship::Context::GetInstance()->GetResourceManager()->LoadFileProcess(
        initData->Path + ext);

        if (filePng != nullptr) {
            return loadPngTexture(filePng, initData);
        }
    }

    auto texture = std::make_shared<Fast::Texture>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    texture->Type = (Fast::TextureType)reader->ReadUInt32();
    texture->Width = reader->ReadUInt32();
    texture->Height = reader->ReadUInt32();
texture->ImageDataSize = reader->ReadUInt32();
    
    // 1. Calculate a safe size with 64 bytes of "buffer" room
    size_t paddedSize = texture->ImageDataSize + 4096; 
    
    // 2. Allocate the padded memory
    texture->ImageData = new uint8_t[paddedSize];
    
    // 3. Clear that memory to zero (this is where memset is used)
    memset(texture->ImageData, 0, paddedSize);

    // 4. Read only the actual data from the file into the start of the buffer
    reader->Read((char*)texture->ImageData, texture->ImageDataSize);

    return texture;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryTextureV1::ReadResource(std::shared_ptr<Ship::File> file,
                                             std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    for (const auto& ext : extension) {
        auto filePng = Ship::Context::GetInstance()->GetResourceManager()->LoadFileProcess(
        initData->Path + ext);

        if (filePng != nullptr) {
            return loadPngTexture(filePng, initData);
        }
    }

    auto texture = std::make_shared<Fast::Texture>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    texture->Type = (Fast::TextureType)reader->ReadUInt32();
    texture->Width = reader->ReadUInt32();
    texture->Height = reader->ReadUInt32();
    texture->Flags = reader->ReadUInt32();
    texture->HByteScale = reader->ReadFloat();
    texture->VPixelScale = reader->ReadFloat();
texture->ImageDataSize = reader->ReadUInt32();
    
    // 1. Calculate a safe size with 64 bytes of "buffer" room
    size_t paddedSize = texture->ImageDataSize + 4096; 
    
    // 2. Allocate the padded memory
    texture->ImageData = new uint8_t[paddedSize];
    
    // 3. Clear that memory to zero (this is where memset is used)
    memset(texture->ImageData, 0, paddedSize);

    // 4. Read only the actual data from the file into the start of the buffer
    reader->Read((char*)texture->ImageData, texture->ImageDataSize);

    return texture;
}
} // namespace Fast

