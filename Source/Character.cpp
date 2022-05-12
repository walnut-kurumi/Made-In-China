#include "Character.h"
#include "Collision.h"
#include "StageManager.h"
#include "DoorManager.h"
#include "Camera/CameraManager.h"

void Character::UpdateTransform() {
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    // 回転行列を作成
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;

    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // 3つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

void Character::Move(float vx, float vz, float speed) {
    // 移動方向ベクトル設定
    moveVecX = vx;
    moveVecZ = vz;

    // 最大速度設定
    maxMoveSpeed = speed;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed) {
    speed *= elapsedTime;

    // 進行ベクトルがゼロベクトルの場合は処理する必要なし
    if (vx == 0.0f && vz == 0.0f) return;

    // 進行ベクトルを単位ベクトル化
    float length = sqrtf(vx * vx + vz * vz);
    vx /= length;
    vz /= length;

    // 自身の回転値から前方向を求める
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    // 回転角を求めるため、2つの単位ベクトルの内積を計算する
    float dot = (vx * frontX) + (vz * frontZ);

    // 内積値は-1.0~1.0で表現されており、2つの単位ベクトルの角度が
    // 小さいほど1.0に近づくという性質を利用して回転速度を調整する
    float rot = 1.0f - dot;

    // 左右判定を行うために2つの単位ベクトルの外積を計算する
    float cross = (vx * frontZ) - (vz * frontX);

    // 2Dの外積値が正の場合か負の場合によって左右判定が行える
    // 左右判定を行うことによって左右判定を選択する
    if (cross < 0.0f) {
        angle.y -= rot * speed;
    }
    else {
        angle.y += rot * speed;
    }
}

void Character::Jump(float speed) {
    //上方向の力を設定
    velocity.y = speed;
}

void Character::UpdateSpeed(float elapsedTime) {
    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime;

    // 速力更新処理
    UpdateVelocity(elapsedFrame);

    // 移動更新処理
    UpdateMove(elapsedTime);
}

bool Character::ApplyDamage(int damage, float invincibleTime) {
    // ダメージが0の場合は健康状態を変更する必要がない
    if (damage <= 0) return false;

    // 死亡している場合は健康状態を変更しない
    if (health <= 0) return false;

    if (invincibleTimer > 0) return false;

    invincibleTimer = invincibleTime;

    // ダメージ処理
    health -= damage;

    // 健康状態が変更した場合はtrueを返す
    return true;
}

//  無敵時間更新処理
void Character::UpdateInvincibleTimer(float elapsedTime) {
    if (invincibleTimer > 0.0f) invincibleTimer -= elapsedTime;
}

void Character::UpdateVerticalVelocitiy(float elapsedFrame) {
    // 移動処理
    if(moveVecY) velocity.y = moveVecY * elapsedFrame* maxMoveSpeed;
    if(gravFlag) velocity.y += gravity * 1.75f * elapsedFrame;

    // 最大値処理
    if (velocity.y < downMax) velocity.y = downMax;
}

void Character::UpdateVerticalMove(float elapsedTime) {
    // 垂直方向の移動量
    float my = velocity.y * elapsedTime;

    slopeRate = 0.0f;

    // 落下中
    if (my < 0.0f) {
        // レイの開始位置は足元より少し上
        DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

        // レイキャストによる地面判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            // 貫通タイプかつ、貫通モードなら
            if (penetrate&&hit.penetrate) {
                position.y += my;
                isGround = false;
                return;
            }
            // 地面に接地している
            position.x = hit.position.x;
            position.y = hit.position.y;
            position.z = hit.position.z;

            // 回転
            angle.y += hit.rotation.y;

            //  傾斜率の計算
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            // 着地した
            if (!isGround) OnLanding();

            isGround = true;
            velocity.y = 0.0f;
        }
        else if (DoorManager::Instance().RayCast(start, end, hit)) {
            // 貫通タイプかつ、貫通モードなら
            if (penetrate && hit.penetrate) {
                position.y += my;
                isGround = false;
                return;
            }
            // 地面に接地している
            position.x = hit.position.x;
            position.y = hit.position.y;
            position.z = hit.position.z;

            // 回転
            angle.y += hit.rotation.y;

            //  傾斜率の計算
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            // 着地した
            if (!isGround) OnLanding();

            isGround = true;
            velocity.y = 0.0f;
        }
        else{
            // 空中に浮いてる
            position.y += my;
            isGround = false;
        }
    }   
    // 上昇中
    else if (my > 0.0f) {
        // レイの開始位置は頭
        DirectX::XMFLOAT3 start = { position.x, position.y + height, position.z };
        // レイの終点位置は移動後の位置
        DirectX::XMFLOAT3 end = { position.x, position.y + height + my, position.z };

        // レイキャストによる天井判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            // 貫通タイプなら貫通
            if (hit.penetrate) {
                position.y += my;
                return;
            }
            // 天井に接している
            position.x = hit.position.x;
            position.y = hit.position.y - height;
            position.z = hit.position.z;

            // 回転
            angle.y += hit.rotation.y;

            velocity.y = 0.0f;
        }
        else if (DoorManager::Instance().RayCast(start, end, hit)) {
            // 貫通タイプなら貫通
            if (hit.penetrate) {
                position.y += my;
                return;
            }
            // 天井に接している
            position.x = hit.position.x;
            position.y = hit.position.y - height;
            position.z = hit.position.z;

            // 回転
            angle.y += hit.rotation.y;

            velocity.y = 0.0f;
        }
        else {
            position.y += my;
        }
    }
}

// 衝撃を与える
void Character::AddImpulse(const Vec3& impulse) {
    // 速力に力を加える
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

#include <math.h>

// 水平速力更新処理
void Character::UpdateHorizontalVelocity(float elapsedFrame) {
    // XZ平面の速力を減速する
    float length = sqrtf(velocity.x * velocity.x);
    if (length > 0.0f) {
        // 摩擦力
        float friction = this->friction * elapsedFrame;
    
        // 空中にいるときは摩擦力を減らす
        if (!isGround) friction -= airControl;
    
        // 摩擦による横方向の速度処理
        if (length > friction) {
            float vx = velocity.x / length;
            velocity.x -= vx * friction;
        }
        // 横方向の速力が摩擦力以下になったので速力を無効化
        else {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }
    
    // XZ平面の速力を加速する
    if (length <= maxMoveSpeed) {
        // 移動ベクトルがゼロベクトルでないなら加速する
        float moveVecLength = sqrtf(moveVecX * moveVecX);
        if (moveVecLength > 0.0f) {
            // 加速力
            float acceleration = this->acceleration * elapsedFrame;
    
            // 移動ベクトルによる加速処理
            velocity.x += acceleration * moveVecX;
    
            //  下り坂でガタガタしないようにする
            if (isGround && slopeRate > 0.0f) {
                velocity.y -= length * slopeRate * elapsedFrame;
            }
    
            // 最大速度制限
            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed) {
                float vx = velocity.x / length;
                velocity.x = vx * maxMoveSpeed;
            }
        }
    }
    //移動ベクトルをリセット
    moveVecX *= 0.0f;
    moveVecZ *= 0.0f;
}

// 水平移動更新処理
void Character::UpdateHorizontalMove(float elapsedTime) {
     // 水平速力計算
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    
    // 移動した方向
    direction = VecMath::sign(velocity.x);

    if (velocityLengthXZ > 0.0f) {
        // 水平移動値
        float mx = velocity.x * elapsedTime;
        float mz = velocity.z * elapsedTime;

        // レイの開始位置と終点位置
        DirectX::XMFLOAT3 start = { position.x , position.y + stepOffset, position.z };
        DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };
        // レイの開始位置と終点位置(頭)
        DirectX::XMFLOAT3 start2 = { position.x , position.y + stepOffset + height, position.z };
        DirectX::XMFLOAT3 end2 = { position.x + mx, position.y + stepOffset + height, position.z + mz };

        // レイキャストによる壁判定
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit)) {
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影                          // ベクトルの否定する
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // 補正位置の計算                   // v1 ベクトル乗数 v2 ベクター乗算   3 番目のベクトルに追加された最初の 2 つのベクトルの積を計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // 戻り値 ベクトルの積和を返す    戻り値　＝　v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // 補正位置が壁に埋まっているかどうか
            if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2)) {
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }

        }
        else if (StageManager::Instance().RayCast(start2, end2, hit)) {
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start2);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end2);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影                          // ベクトルの否定する
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // 補正位置の計算                   // v1 ベクトル乗数 v2 ベクター乗算   3 番目のベクトルに追加された最初の 2 つのベクトルの積を計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // 戻り値 ベクトルの積和を返す    戻り値　＝　v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // 補正位置が壁に埋まっているかどうか
            if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2)) {
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
            
        }        
        else if (DoorManager::Instance().RayCast(start, end, hit)) {
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影                          // ベクトルの否定する
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // 補正位置の計算                   // v1 ベクトル乗数 v2 ベクター乗算   3 番目のベクトルに追加された最初の 2 つのベクトルの積を計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // 戻り値 ベクトルの積和を返す    戻り値　＝　v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // 補正位置が壁に埋まっているかどうか
            if (!DoorManager::Instance().RayCast(hit.position, collectPosition, hit2)) {
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }

        }
        else if (DoorManager::Instance().RayCast(start2, end2, hit)) {
            // 壁までのベクトル
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start2);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end2);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            // 壁の法線
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを法線に射影                          // ベクトルの否定する
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

            // 補正位置の計算                   // v1 ベクトル乗数 v2 ベクター乗算   3 番目のベクトルに追加された最初の 2 つのベクトルの積を計算
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End); // 戻り値 ベクトルの積和を返す    戻り値　＝　v1 * v2 + v3
            DirectX::XMFLOAT3 collectPosition;
            DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

            HitResult hit2; // 補正位置が壁に埋まっているかどうか
            if (!DoorManager::Instance().RayCast(hit.position, collectPosition, hit2)) {
                position.x = collectPosition.x;
                position.z = collectPosition.z;
            }
            else {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }

        }
        else {
            position.x += mx;
            position.z += mz;
        }
    }
}

void Character::UpdateVelocity(float elapsedFrame) {
    UpdateVerticalVelocitiy(elapsedFrame);
    UpdateHorizontalVelocity(elapsedFrame);
}

void Character::UpdateMove(float elapsedTime) {
    UpdateVerticalMove(elapsedTime);
    UpdateHorizontalMove(elapsedTime);
}
