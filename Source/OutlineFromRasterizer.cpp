#include "OutlineFromRasterizer.h"

// static変数は宣言必要
Microsoft::WRL::ComPtr<ID3D11RasterizerState> RenderStateKARI::rasterizer[CU_END];