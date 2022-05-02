#include "SceneGame.h"
#include "Camera/CameraManager.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

#include "EnemyManager.h"
#include "EnemyGunner.h"
#include "EnemyMelee.h"

#include"SceneLoading.h"
#include"SceneClear.h"
#include"SceneOver.h"

#include "Menu.h"

#include "StageManager.h"
#include "StageSkybox.h"
#include "StageMain.h"
#include "StageCollision.h"

#include "Framework.h"

#include"EffectManager.h"
#include"Graphics/Texture.h"


// ������
void SceneGame::Initialize()
{
    // ���[�h��������    
    SetLoadPercent(0.0f);

    HRESULT hr{ S_OK };

    ID3D11Device* device = Graphics::Ins().GetDevice();
    Graphics& gfx = Graphics::Ins();

    //Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
    //D3D11_TEXTURE2D_DESC texture2dDesc{};
    //texture2dDesc.Width = gfx.GetScreenWidth();
    //texture2dDesc.Height = gfx.GetScreenHeight();
    //texture2dDesc.MipLevels = 1;
    //texture2dDesc.ArraySize = 1;
    //texture2dDesc.Format = DXGI_FORMAT_R32_TYPELESS; // DXGI_FORMAT_R24G8_TYPELESS DXGI_FORMAT_R32_TYPELESS
    //texture2dDesc.SampleDesc.Count = 1;
    //texture2dDesc.SampleDesc.Quality = 0;
    //texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    //texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    //texture2dDesc.CPUAccessFlags = 0;
    //texture2dDesc.MiscFlags = 0;
    //hr = device->CreateTexture2D(&texture2dDesc, NULL, depth_stencil_buffer.GetAddressOf());
    //_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // �v���C���[
    player = std::make_unique<Player>(device);
    player->Init(); 

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �X�e�[�W
    {
        StageManager::Create();
        StageManager::Instance().Init();

        // ���[�h���X�V
        AddLoadPercent(1.0f);

        StageMain* stageMain = new StageMain(device);
        stageMain->PlayerData(player.get());
        StageManager::Instance().Register(stageMain);
        StageCollision* stageCollision = new StageCollision(device);
        StageManager::Instance().Register(stageCollision);
        StageSkybox* skybox = new StageSkybox(device);
        StageManager::Instance().Register(skybox);
    }
    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �G�l�~�[���W�ݒ�
    EnemyPositionSetting();

    // �G�l�~�[������			    
    EnemyInitialize(device);

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    // �}�E�X�J�[�\�����������ǂ���
    Input::Instance().GetMouse().SetMoveCursor(true);

    // CAMERA_SHAKE
    // TODO:02 Create a constant buffer object.
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

   /* buffer_desc.ByteWidth = sizeof(POST_EFFECT_CONSTANTS);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, postConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    buffer_desc.ByteWidth = sizeof(BLOOM_CONSTANTS);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, bloomConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    buffer_desc.ByteWidth = sizeof(MIST_CONSTANTS);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, mistConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));



    rasterizer = std::make_unique<Rasterizer>(device);
    depth = std::make_unique<DeppthSteciler>(device);
    blender = std::make_unique<Blender>(device);*/


    Bar = new Sprite(device, L"./Data/Sprites/Load/Bar.png");
    LoadBar = new Sprite(device, L"./Data/Sprites/Load/LoadBar.png");   
    enemyattack = new Sprite(device, L"./Data/Sprites/enemyattack.png");

    Menu::Instance().Initialize();

    // ���[�h���X�V
    AddLoadPercent(1.0f);

    Fade::Instance().Initialize();

    //�f�o�b�O
    //hitEffect = new Effect("Data/Effect/player_hit.efk");
   /* framebuffers[static_cast<size_t>(FRAMEBUFFER::SCENE_RESOLVED)] = std::make_unique<framebuffer>(device, gfx.GetScreenWidth(), gfx.GetScreenHeight());
    framebuffers[static_cast<size_t>(FRAMEBUFFER::POST_PROCESSED)] = std::make_unique<framebuffer>(device, gfx.GetScreenWidth(), gfx.GetScreenHeight(), FB_FLAG::COLOR);

    bitBlockTransfer = std::make_unique<fullscreen_quad>(device);
    postEffectPs.initialize(device, "shader\\obj\\post_effect_ps.cso");
    toonMapPs.initialize(device, "shader\\obj\\tone_map_ps.cso");

    shaderResourceViews[static_cast<size_t>(TEXTURE::DISTORTION)].Attach(queryTexture(device, L"./Data/Sprites/distortion.png", &texture2dDesc));

    bloom_effect = std::make_unique<bloom>(device, gfx.GetScreenWidth(), gfx.GetScreenHeight());*/

    // ���[�h�� 100%
    SetLoadPercent(10.0f);
}

// �I����
void SceneGame::Finalize()
{
    // �G�l�~�[�I������	
    EnemyManager::Instance().Clear();
    // �X�e�[�W�I������
    StageManager::Instance().Clear();
    StageManager::Destory();    

    Menu::Instance().Finalize();

    // �J�����V�F�C�N�i�ȑf�j
    CameraManager& cameraMgr = CameraManager::Instance();
    cameraMgr.SetShakeFlag(false);

    delete enemyattack;
    delete LoadBar;
    delete Bar;
    //�f�o�b�O
   // delete hitEffect;
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    if (Menu::Instance().GetMenuFlag() == false)
    {

        float slowElapsedTime = elapsedTime * player->GetPlaybackSpeed();
        //TODO: �G�̐��������elapsedTime�@���������Ȃ�
        // �q�b�g�X�g�b�v
        slowElapsedTime = slowElapsedTime * player->GetHitStopSpeed();
        // �X���[���[�V����
        slowElapsedTime = slowElapsedTime * player->GetPlaybackSpeed();



        DirectX::XMFLOAT3 screenPosition;
        screenPosition.x = static_cast<float>(mouse.GetPositionX());
        screenPosition.y = static_cast<float>(mouse.GetPositionY());


        // �X�e�[�W
        StageManager::Instance().Update(slowElapsedTime);


        // �v���C���[
        {
            player->Update(slowElapsedTime);
            // �V�t�g�u���C�N�X�V����
            SBManager::Instance().Update(slowElapsedTime);
        }

        // �J����
        {
            CameraManager& cameraMgr = CameraManager::Instance();

            cameraMgr.Update(slowElapsedTime);

            Vec3 target = player->GetPosition() + VecMath::Normalize(Vec3(player->GetTransform()._21, player->GetTransform()._22, player->GetTransform()._23)) * 7.5f;
            CameraManager::Instance().SetTarget(target);
        }


        // �G�l�~�[
        if(!player->GetClock())
        {
            EnemyManager::Instance().SetPlayer(player.get());
            // �\�[�g
            EnemyManager::Instance().SortLengthSq(player->GetPosition());
            // �G�l�~�[�X�V����
            EnemyManager::Instance().Update(slowElapsedTime);
            // �e�ۍX�V����
            EnemyBulletManager::Instance().Update(slowElapsedTime);

        }

        //�G�t�F�N�g�X�V����
        EffectManager::Instance().Update(slowElapsedTime);


        // �X���[���ԕ\��
        w = player->GetSlowTimer() / player->GetSlowMax();
        et = elapsedTime;
    }

    // ���Z�b�g
    if (player->GetHealth() <= 0)// ||  gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        // �t�F�[�h�A�E�g
        if (!Fade::Instance().GetFadeOutFinish())Fade::Instance().SetFadeOutFlag(true);

        // �t�F�[�h�A�E�g���������
        if (Fade::Instance().GetFadeOutFinish())
        {
            // ���Z�b�g
            // �f�o�b�O�p�ŏ����Ă�
            Reset();

            // �t�F�[�h�C��
            Fade::Instance().SetFadeInFlag(true);
        }
    }        
    // �t�F�[�h�C���I������珉����
    if (Fade::Instance().GetFadeInFinish())Fade::Instance().Initialize();

    // Menu
    Menu::Instance().Update(elapsedTime);
    // Fade
    Fade::Instance().Update(elapsedTime);

    //�G�t�F�N�g�f�o�b�O
   /* const mouseButton mouseClick =
        Mouse::BTN_LEFT;
    if(mouse.GetButtonDown() & mouseClick && a == false)
        handle = hitEffect->Play(player->GetPosition()); a = true; ti = 0;

    if (ti %= 300)
        hitEffect->Stop(handle); a = false;

    ti++;*/

    // TODO ���݂̃X�e�[�W�̎���ł�G�l�~�[�̐����O�̏ꍇ ���̃X�e�[�W��
    if (EnemyManager::Instance().GetDeadEnemyCount() >= EnemyManager::Instance().GetEnemyCount())
    {
        // ���̃X�e�[�W�ֈڂ鏈��
        SceneManager::Instance().ChangeScene(new SceneLoading(new SceneClear));
    }
}

// �`�揈��
void SceneGame::Render(float elapsedTime)
{
    Graphics& gfx = Graphics::Ins();
    ID3D11Device* device = gfx.GetDevice();
    ID3D11DeviceContext* dc = gfx.GetDeviceContext();    
    CameraManager& cameraMgr = CameraManager::Instance();

    //DirectX::XMFLOAT4X4 data{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };


    //// TODO:05 Bind the transformation matrix data to the vertex shader at register number 0.
    //dc->UpdateSubresource(constant_buffer, 0, 0, &data, 0, 0);
    //dc->VSSetConstantBuffers(3, 1, &constant_buffer);

    //// UNIT.32
    //dc->UpdateSubresource(postConstantBuffer.Get(),0,0,&postEffectConstant,0,0);
    //dc->PSSetConstantBuffers(8, 1, postConstantBuffer.GetAddressOf());
    //// UNIT.99
    //dc->UpdateSubresource(bloomConstantBuffer.Get(), 0, 0, &bloomConstant, 0, 0);
    //dc->PSSetConstantBuffers(6, 1, bloomConstantBuffer.GetAddressOf());
    //// UNIT.99
    //dc->UpdateSubresource(mistConstantBuffer.Get(), 0, 0, &mistConstant, 0, 0);
    //dc->PSSetConstantBuffers(7, 1, mistConstantBuffer.GetAddressOf());
 
    // ���f���`��
    {
        // �X�e�[�W�`��
        StageManager::Instance().Render(dc, elapsedTime);
        // �v���C���[�`��
        player->Render(dc);
        // �G�l�~�[�`��
        EnemyManager::Instance().Render(dc, &Shaders::Ins()->GetSkinnedMeshShader());
    }
   
    // �f�o�b�N
    {
        player->DrawDebugGUI();        
    }
    
    //3D�G�t�F�N�g�`��
    {
        EffectManager::Instance().Render(cameraMgr.GetView(), cameraMgr.GetProjection());
    }

    //// Apply post-effect processing. Includes shadows, bokeh, mist, bloom, etc.
    //framebuffers[static_cast<size_t>(FRAMEBUFFER::POST_PROCESSED)]->clear(dc, FB_FLAG::COLOR);
    //framebuffers[static_cast<size_t>(FRAMEBUFFER::POST_PROCESSED)]->activate(dc, FB_FLAG::COLOR);
    //depth->setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF, dc);
    //blender->setBlendMode(Blender::BLEND_STATE::BS_NONE, dc);
    //rasterizer->setRasterMode(Rasterizer::RASTER_STATE::CULL_NONE, dc);
    //dc->PSSetShaderResources(static_cast<UINT>(TEXTURE::DISTORTION), 1, shaderResourceViews[static_cast<size_t>(TEXTURE::DISTORTION)].GetAddressOf());
    //ID3D11ShaderResourceView* post_effects_views[]{
    //    framebuffers[static_cast<size_t>(FRAMEBUFFER::SCENE_RESOLVED)]->color_map().Get(),
    //    framebuffers[static_cast<size_t>(FRAMEBUFFER::SCENE_RESOLVED)]->depth_map().Get() };
    //bitBlockTransfer->blit(dc, post_effects_views, 0, _countof(post_effects_views), postEffectPs.getShader());
    //framebuffers[static_cast<size_t>(FRAMEBUFFER::POST_PROCESSED)]->deactivate(dc);

    //// Extracts high-luminance components and generates blurred images.
    //depth->setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF, dc);
    //blender->setBlendMode(Blender::BLEND_STATE::BS_NONE, dc);
    //rasterizer->setRasterMode(Rasterizer::RASTER_STATE::CULL_NONE, dc);
    //bloom_effect->make(dc, framebuffers[static_cast<size_t>(FRAMEBUFFER::POST_PROCESSED)]->color_map().Get());

    //framebuffers[static_cast<size_t>(FRAMEBUFFER::POST_PROCESSED)]->activate(dc, FB_FLAG::COLOR);
    //depth->setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF, dc);
    //blender->setBlendMode(Blender::BLEND_STATE::BS_ADD, dc);
    //rasterizer->setRasterMode(Rasterizer::RASTER_STATE::CULL_NONE, dc);
    //bloom_effect->blit(dc);
    //framebuffers[static_cast<size_t>(FRAMEBUFFER::POST_PROCESSED)]->deactivate(dc);

    //// Tone mapping
    //depth->setRasterMode(DeppthSteciler::DEPTH_STENCIL_STATE::ZT_OFF_ZW_OFF, dc);
    //blender->setBlendMode(Blender::BLEND_STATE::BS_NONE, dc);
    //rasterizer->setRasterMode(Rasterizer::RASTER_STATE::CULL_NONE, dc);
    //bitBlockTransfer->blit(dc, framebuffers[static_cast<size_t>(FRAMEBUFFER::POST_PROCESSED)]->color_map().GetAddressOf(), 0, 1, toonMapPs.getShader());

    // 2D�`��
    {
        // �U���\���`��
        RenderEnemyAttack();

        // UI
        Bar->render(dc, 600, 650, 620, 25, 1.0f, 1.0f, 1.0f, 1.0f, 0);
        LoadBar->render(dc, 605, 652, 605 * w, 21, 1.0f, 1.0f, 1.0f, 1.0f, 0);

        // ���j���[
        Menu::Instance().Render(elapsedTime);

        // �t�F�[�h�p
        Fade::Instance().Render(elapsedTime);

    }


#ifdef USE_IMGUI   


    ImGui::Begin("ImGUI");

    // CAMERA_SHAKE
    // TODO:07 Adjust the maximum amount of rotation(max_skew) and movement(max_sway) of the camera.
    ImGui::SliderFloat("max_sway [pixel]", &max_sway, 0.0f, 64.0f);
    ImGui::SliderFloat("max_skew [degree]", &max_skew, 0.0f, 10.0f);
    // TODO:12 Defines the amount of seed shifting factor for perlin noise.
    ImGui::SliderFloat("seed_shifting_factor", &seed_shifting_factor, 0.0f, 10.0f);

    ImGui::SliderFloat("elapsedTime", &et, 0.0f, 1.0f);
    
    bool sh = cameraMgr.GetShakeFlag();
    ImGui::Checkbox("shakeFlag", &sh);

    /*if (ImGui::CollapsingHeader("bloom configuration"))
    {
        ImGui::SliderFloat("bloom_extraction_threshold", &bloomConstant.data.bloom_extraction_threshold, +0.0f, +10.0f);
        ImGui::SliderFloat("blur_convolution_intensity", &bloomConstant.data.blur_convolution_intensity, +0.0f, +10.0f);
    }
    if (ImGui::CollapsingHeader("mist configuration"))
    {
        ImGui::SliderFloat("extinction:mist_density", &mistConstant.data.mist_density[0], 0.0f, 1.0f);
        ImGui::SliderFloat("extinction:mist_height_falloff", &mistConstant.data.mist_height_falloff[0], -1000.0f, 1000.0f);
        ImGui::SliderFloat("extinction:height_mist_offset", &mistConstant.data.height_mist_offset[0], -1000.0f, 1000.0f);
        ImGui::SliderFloat("inscattering:mist_density", &mistConstant.data.mist_density[1], 0.0f, 1.0f);
        ImGui::SliderFloat("inscattering:mist_height_falloff", &mistConstant.data.mist_height_falloff[1], -1000.0f, 1000.0f);
        ImGui::SliderFloat("inscattering:height_mist_offset", &mistConstant.data.height_mist_offset[1], -1000.0f, 1000.0f);
        ImGui::SliderFloat("mist_cutoff_distance", &mistConstant.data.mist_cutoff_distance, 0.0f, 500.0f);
        ImGui::ColorEdit3("mist_color", mistConstant.data.mist_color);

        ImGui::SliderFloat("mist_flow_speed", &mistConstant.data.mist_flow_speed, 0.0f, 500.0f);
        ImGui::SliderFloat("mist_flow_noise_scale_factor", &mistConstant.data.mist_flow_noise_scale_factor, 0.0f, 0.1f);
        ImGui::SliderFloat("mist_flow_density_lower_limit", &mistConstant.data.mist_flow_density_lower_limit, 0.0f, 1.0f);

        ImGui::SliderFloat("distance_to_sun", &mistConstant.data.distance_to_sun, 0.0f, 1000.0f);
        ImGui::SliderFloat("sun_highlight_exponential_factor", &mistConstant.data.sun_highlight_exponential_factor, 0.0f, 500.0f);
        ImGui::SliderFloat("sun_highlight_intensity", &mistConstant.data.sun_highlight_intensity, 0.0f, 100.0f);
    }
    if (ImGui::CollapsingHeader("bokeh configuration"))
    {
        ImGui::SliderFloat("bokeh_aperture", &postEffectConstant.data.bokeh_aperture, 0.0f, 0.1f);
        ImGui::SliderFloat("bokeh_focus", &postEffectConstant.data.bokeh_focus, 0.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("color adjustment"))
    {
        ImGui::SliderFloat("brightness", &postEffectConstant.data.brightness, -1.0f, +1.0f);
        ImGui::SliderFloat("contrast", &postEffectConstant.data.contrast, -1.0f, +1.0f);
        ImGui::SliderFloat("hue", &postEffectConstant.data.hue, -1.0f, +1.0f);
        ImGui::SliderFloat("saturation", &postEffectConstant.data.saturation, -1.0f, +1.0f);
    }

    ImGui::ColorEdit3("pantone", postEffectConstant.data.pantone);
    ImGui::SliderFloat("intensity", &postEffectConstant.data.pantone[3], +0.0f, +10.0f);

    ImGui::SliderFloat("post_effect_options[0]", &postEffectConstant.data.post_effect_options, 0.0f, 1.0f);*/

    ImGui::End();

   /* ImGui::SetNextWindowPos(ImVec2(0, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(30, 30), ImGuiCond_FirstUseEver);    

    if (ImGui::Begin("SRV", nullptr, ImGuiWindowFlags_None))
    {        
        ImGui::Image(shaderResourceViews[0], { 320, 180 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });            
        ImGui::Image(shaderResourceViews[1], { 320, 180 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });            
        ImGui::Image(shaderResourceViews[2], { 320, 180 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });              
    }
    ImGui::End();*/
#endif
    
}

// player�����񂾂Ƃ� ���̃��Z�b�g�p
void SceneGame::Reset()
{
    // ���܂Ȃ�
    EnemyBulletManager::Instance().Clear();
    SBManager::Instance().Clear();
    // �G�h�� �|�W�V�������Z�b�g
    EnemyManager::Instance().Init();    
    EnemyManager::Instance().EnemyReset();

    // �v���C���[�h�� �|�W�V�������Z�b�g
    player->Init();
    

}

// �G�̏�����
void SceneGame::EnemyInitialize(ID3D11Device* device)
{
    for (int i = 0; i < ENEMY_MAX; i++)
    {
        if (ENEMY_MAX / 2 == i)
        {
            // ���[�h���X�V
            AddLoadPercent(1.0f);
        }
        if (ENEMY_MAX == i)
        {
            // ���[�h���X�V
            AddLoadPercent(1.0f);
        }

        // �ߐڂƉ��u�����݂�
        if (i % 2 == 0)
        {
            EnemyGunner* gunner = new EnemyGunner(device);
            gunner->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));            
            gunner->PositionInitialize();

            //������邩�ǂ���
            gunner->SetInitialWalk(enemyWalk[i]);
            gunner->WalkFlagInitialize();

            // �O���[�v�ԍ��Z�b�g
            gunner->SetInitialGroupNum(enemyGroup[i]);
            gunner->GroupNumInitialize();            

            EnemyManager::Instance().Register(gunner);
        }
        else
        {
            EnemyMelee* melee = new EnemyMelee(device);
            melee->SetInitialPos(Vec3(enemyPos[i].x, enemyPos[i].y, 0));
            melee->PositionInitialize();

            //������邩�ǂ���
            melee->SetInitialWalk(enemyWalk[i]);
            melee->WalkFlagInitialize();

            // �O���[�v�ԍ��Z�b�g
            melee->SetInitialGroupNum(enemyGroup[i]);
            melee->GroupNumInitialize();

            EnemyManager::Instance().Register(melee);
        }

        EnemyManager::Instance().Init();
    }
}

// �G�l�~�[���W�ݒ�
void SceneGame::EnemyPositionSetting()
{

    enemyPos[0] = {-75,0.5f};
    enemyPos[1] = {-85,0.5f};
    enemyPos[2] = {-105,0.5f};
    enemyPos[3] = {-120,0.5f};
    enemyPos[4] = {-40,20.5f};
    enemyPos[5] = {4,42.5f};
    enemyPos[6] = {-65,55.5f};
    enemyPos[7] = {-130,85.5f};
    enemyPos[8] = {20,70.5f};    


    enemyGroup[0] =0;
    enemyGroup[1] =0;
    enemyGroup[2] =0;
    enemyGroup[3] =0;  
    enemyGroup[4] =1;
    enemyGroup[5] =1;
    enemyGroup[6] =2;
    enemyGroup[7] =2;
    enemyGroup[8] =2;

    enemyWalk[0] = true;
    enemyWalk[1] = true;
    enemyWalk[2] = true;
    enemyWalk[3] = true;
    enemyWalk[4] = false;
    enemyWalk[5] = false;
    enemyWalk[6] = false;
    enemyWalk[7] = false;
    enemyWalk[8] = false;
}


// �G�l�~�[�U���\���`��
void SceneGame::RenderEnemyAttack()
{
    
    ID3D11DeviceContext* dc = Graphics::Ins().GetDeviceContext();

    // �r���[�|�[�g
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &viewport);
    // �ϊ��s��
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetView());
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetProjection());
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
    // �S�Ă̓G�̓����HP�Q�[�W��\��
    EnemyManager& enemyManager = EnemyManager::Instance();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);
        DirectX::XMFLOAT3 worldPosition = enemy->GetCenterPosition();
        worldPosition.y += enemy->GetHeight();
        DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
        // ���[���h���W����X�N���[�����W�֕ϊ�
        DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
            WorldPosition,
            viewport.TopLeftX,
            viewport.TopLeftY,
            viewport.Width,
            viewport.Height,
            viewport.MinDepth,
            viewport.MaxDepth,
            Projection,
            View,
            World
        );
        // �X�N���[�����W
        DirectX::XMFLOAT2 screenPosition;
        DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);                

        if (enemy->GetIsAttack() && enemy->GetHealth() > 0)
        {
            // �U���\���`��
            enemyattack->render(dc, screenPosition.x - 32, screenPosition.y - 64, 64, 64, 1, 1, 1, 1, 0);
        }
    }   
}