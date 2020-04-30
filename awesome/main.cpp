#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#ifdef _DEBUG
#include <iostream>
#endif

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace std;

constexpr auto window_width = 500;
constexpr auto window_height = 500;

// @brief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
// @param format �t�H�[�}�b�g(%d�Ƃ�%f�Ƃ���)
// @param �ϒ�����
// @remarks ���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�܂���
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf(format, valist);
	va_end(valist);
#endif
}

void re_check(HRESULT r) {
	if (r == S_OK) {
		DebugOutputFormatString("�������܂���\n");
	}
	else {
		DebugOutputFormatString("���s���܂����@�I�����܂�");
		exit(1);
	}

}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//�E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);//os�ɑ΂��āu�������̃A�v���͏I���v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//����̏������s���B
}

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
	//�E�B���h�E�N���X�̐������o�^
	DebugOutputFormatString("�E�B���h�E�N���X�̐������o�^\n");

	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = ("DX12sample");			//�A�v���P�[�V�����N���X��(�K���ł悢)
	w.hInstance = GetModuleHandle(nullptr);		//�n���h���̎擾
	RegisterClassEx(&w);						//�A�v���P�[�V�����N���X(�E�B���h�E�N���X�̎w���os�ɓ`����)

	RECT wrc = { 0,0,window_width,window_height };	//�E�B���h�E�T�C�Y�����߂�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�֐����g���ăE�B���h�E�T�C�Y��␳����

//�E�B���h�E�I�u�W�F�N�g�̐���
DebugOutputFormatString("�E�B���h�E�I�u�W�F�N�g�̐���\n");
	
	HWND hwnd = CreateWindow(w.lpszClassName,	//�N���X���w��
		("DX12�e�X�g"),							//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,					//�^�C	�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,							//�\��x���W��os�ɂ��C��
		CW_USEDEFAULT,							//�\��y���W��os�ɂ��C��
		wrc.right - wrc.left,					//�E�B���h�E��
		wrc.bottom - wrc.top,					//�E�B���h�E��
		nullptr,								//�e�E�B���h�E�n���h��
		nullptr,								//���j���[�n���h��
		w.hInstance,							//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);								//�ǉ��p�����[�^

//�E�B���h�E�\��
DebugOutputFormatString("�E�B���h�E�\��\n");

	ShowWindow(hwnd, SW_SHOW);

//Direct3D DXGI�̐ݒ�
DebugOutputFormatString("Direct3D DXGI�̐ݒ�\n");
	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgifactory = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

//Direct3D�f�o�C�X�̏�����
	D3D_FEATURE_LEVEL featureLevel;

	for (auto lv : levels) {
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			DebugOutputFormatString( "�f�o�C�X����������܂���\n");
			break;//�����\�ȃo�[�W���������������烋�[�v��ł��؂�
		}	
	}
	if (_dev == nullptr) {
		DebugOutputFormatString("�����I�����܂�\n");
		exit(1);
	}
//DXGIFactory�I�u�W�F�N�g�̐���
DebugOutputFormatString("DXGIFactory�I�u�W�F�N�g�̐���\n");
	re_check(CreateDXGIFactory1(IID_PPV_ARGS(&_dxgifactory)));

//�R�}���h���X�g�ƃR�}���h�A���P�[�^
DebugOutputFormatString("�R�}���h���X�g�ƃR�}���h�A���P�[�^�쐬\n");
	ID3D12CommandAllocator*  _cmdAllocator = nullptr;	
	ID3D12GraphicsCommandList* _cmdList = nullptr;
//�R�}���h�A���P�[�^�̍쐬
DebugOutputFormatString("�R�}���h�A���P�[�^�̍쐬\n");
	re_check(_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&_cmdAllocator)));

//�R�}���h���X�g�̍쐬
DebugOutputFormatString("�R�}���h���X�g�̍쐬\n");
	re_check(_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
	_cmdAllocator, nullptr,
	IID_PPV_ARGS(&_cmdList)));

//�R�}���h�L���[�̍쐬
DebugOutputFormatString("�R�}���h�L���[�̍쐬\n");
	ID3D12CommandQueue* _cmdQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;					//�^�C���A�E�g�Ȃ�
	cmdQueueDesc.NodeMask = 0;											//�A�_�v�^��������g��Ȃ��Ƃ���0�ł悢
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;		//�v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;					//�R�}���h���X�g�ƍ��킹��

	re_check(_dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue)));
//�X���b�v�`�F�[���̐���
DebugOutputFormatString("�X���b�v�`�F�[���̐���\n");

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;				//�o�b�N�o�b�t�@�[�͐L�у`�W�~�\
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//�t���b�v��͑��₩�ɔj��
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;		//���Ɏw��Ȃ�

	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//�E�B���h�E�t���X�N���[���ؑ։\
	re_check(_dxgifactory->CreateSwapChainForHwnd(
		_cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain));
//�f�B�X�N���v�^�q�[�v�̍쐬
DebugOutputFormatString("�f�B�X�N���v�^�q�[�v�̍쐬�iRTV�p�j\n");
	ID3D12DescriptorHeap* rtvHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;			//�����_�[�^�[�Q�b�g�r���[�ɂ���̂�
	heapDesc.NodeMask = 0;									//GPU���������鎞�̐ݒ�Ȃ̂�1�����g�p����z��Ȃ̂�0
	heapDesc.NumDescriptors = 2;							//���\��2��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		//���Ɏw��Ȃ��@�V�F�[�_������Q�Ƃ���K�v�����邩�̎w��

	re_check(_dev->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&rtvHeap)));

//�X���b�v�`�F�[���̃������ƃf�B�X�N���v�^��R�Â���
DebugOutputFormatString("�X���b�v�`�F�[���̃������ƃf�B�X�N���v�^��R�Â���\n");
	
D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();			//RTVheap�̃n���h�����擾����
	std::vector<ID3D12Resource*> _backBuffers(heapDesc.NumDescriptors);
	for (int idx = 0; idx < heapDesc.NumDescriptors; ++idx) {

		re_check(_swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx])));				//�X���b�v�`�F�C���̃o�b�N�o�b�t�@�[���擾����
		
		_dev->CreateRenderTargetView(_backBuffers[idx], nullptr, handle);					//�����_�[�^�[�Q�b�g�r���[���쐬����(�X���b�v�`�F�C���̃o�b�N�o�b�t�@�[��o�^����)



		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	MSG msg = {};
	
//���C�����[�v�J�n
	DebugOutputFormatString("���C�����[�v�J�n\n");

	while(true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�A�v���P�[�V�������I���Ƃ���messagage��WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

//�����N���X�͎g��Ȃ��̂œo�^��������
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}