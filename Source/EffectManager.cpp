#include"Graphics/Graphics.h"
#include"EffectManager.h"

//������
void EffectManager::Initialize()
{
	Graphics& graphics = Graphics::Ins();

	//Effekseer�����_������
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(graphics.GetDevice(),
		graphics.GetDeviceContext(), 2048);

	//Effekseer�}�l�[�W���[����
	effekseerManager = Effekseer::Manager::Create(2048);

<<<<<<< HEAD
	//Effekseer�����_���̊e��ݒ�(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�ɂ����̂����Ă���OK)
=======
	//Effekseer�����_���̊e��ݒ�(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK)
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
<<<<<<< HEAD
	//Effekseer���ł̃��[�_�[�̐ݒ�(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ��OK)
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());
	effekseerManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
=======
	//Effekseer���ł̃��[�_�[�ݒ�(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ��OK)
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12

	//Effekseer��������W�n�Ōv�Z����
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

}

//�I����
void EffectManager::Finalize()
{
	if (effekseerManager != nullptr)
	{
<<<<<<< HEAD
		effekseerManager.Reset();
=======
		effekseerManager->Destroy();
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12
		effekseerManager = nullptr;
	}
	if (effekseerRenderer != nullptr)
	{
<<<<<<< HEAD
		effekseerRenderer.Reset();
=======
		effekseerRenderer->Destroy();
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12
		effekseerRenderer = nullptr;
	}
}

//�X�V����
void EffectManager::Update(float elapsedTime)
{
	//�G�t�F�N�g�X�V����(�����ɂ̓t���[���̌o�ߎ��Ԃ�n��)
<<<<<<< HEAD
	effekseerManager->Update();
	effekseerRenderer->SetTime(elapsedTime / 60.0f);
=======
	effekseerManager->Update(elapsedTime * 60.0f);
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12
}

//�`�揈��
void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
<<<<<<< HEAD
	//�r���[���v���W�F�N�V�����s���Effekseer�����_���ɐݒ�

=======
	//�r���[&�v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	//Effekseer�`��J�n
	effekseerRenderer->BeginRendering();

	//Effekseer�`����s
<<<<<<< HEAD
	//�}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐��쐬���A
	//Draw()�֐������s���鏇���Ő���ł�����
=======
	//�}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��}�l�[�W���[�𕡐��쐬���A
	//Draw()�֐������s���鏇���Ő���o������
>>>>>>> e00caf0c2bf18823ecd619cfa3ec9020f21e7b12
	effekseerManager->Draw();

	//Effekseer�`��I��
	effekseerRenderer->EndRendering();
}