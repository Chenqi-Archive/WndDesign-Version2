#include "image.h"
#include "../system/directx/wic_api.h"


BEGIN_NAMESPACE(WndDesign)

using std::make_shared;


void Image::Load(const wstring& file_name) {
    image = make_shared<ImageResource>(file_name);
    if (image->IsEmpty()) { throw std::invalid_argument("image file not found"); }
}

void Image::Load(void* address, size_t size) {
    image = make_shared<ImageResource>(address, size);
    if (image->IsEmpty()) { throw std::invalid_argument("image file not found"); }
}

bool Image::IsEmpty() const { 
    return image == nullptr || image->IsEmpty(); 
}

const Size Image::GetSize() const {
    if (IsEmpty()) { throw std::invalid_argument("image file not loaded"); }
    return image->GetSize();
}


END_NAMESPACE(WndDesign)