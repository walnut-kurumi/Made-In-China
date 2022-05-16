#pragma once
#include "AfterimageBase.h"
#include <memory>
#include <vector>

/// <summary>
/// 残像オブジェクトのプーリングクラス
/// </summary>
template <class T>
class AfterimagePool
{
public:
    AfterimagePool() {}
    virtual ~AfterimagePool() {}

    // 生成.
    void Create(int size) {
        members = new T[size];
        this->size = size;
    }

    // 破棄.
    void Destroy() {
        delete[] members;
    }

    // 未使用のオブジェクトを取得する.
    T* Recycle() {
        for (int i = 0; i < size; i++) {
            // 未使用の場合
            if (!members[i].exists) {
                members[i].exists = true; // 生存フラグを立てる.
                return &members[i]; // 未使用なので再利用可能.
            }
        }

        // 見つからなかった.
        return nullptr;
    }

    // 更新.
    void Update(float elapsedTime) {
        for (int i = 0; i < size; i++) {
            if (members[i].exists) {
                members[i].Update(elapsedTime); // 生存しているオブジェクトのみ更新.
            }
        }
    }

    // 描画.
    void Render(ID3D11DeviceContext* dc) {
        for (int i = 0; i < size; i++) {
            if (members[i].exists) {
                members[i].Render(dc); // 生存しているオブジェクトのみ描画.
            }
        }
    }

    // 生存数をカウントする.
    int Count() {
        int cnt = 0;
        for (int i = 0; i < size; i++) {
            if (members[i].exists) {
                cnt++; // 生存している.
            }
        }
        return cnt;
    }

protected:
    T* members{};
    int size = 0;
};
