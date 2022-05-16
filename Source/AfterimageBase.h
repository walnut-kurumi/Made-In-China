#pragma once
#include "Model.h"
#include "Graphics/Vec.h"
/// 
/// 残像用オブジェクトの基本クラス
/// 
class AfterimageBase
{
public:
    AfterimageBase() {}
    virtual ~AfterimageBase() {}

    // 生成.
    virtual void Create() {
        exists = false; // 生成時は無効.
        // 他に処理が必要であればオーバーライドする.
    }

    virtual void Init() {}   // 初期化.
    virtual void Update(float elapsedTime) {} // 更新.
    virtual void Render(ID3D11DeviceContext* dc) {} // 描画.

public:
    bool  exists = false;   // 生存フラグ.
    float   timer = 0;      // 汎用タイマー.
};
