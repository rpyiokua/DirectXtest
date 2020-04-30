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

// @brief コンソール画面にフォーマット付き文字列を表示
// @param format フォーマット(%dとか%fとかの)
// @param 可変長引数
// @remarks この関数はデバッグ用です。デバッグ時にしか動作しません
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
		DebugOutputFormatString("成功しました\n");
	}
	else {
		DebugOutputFormatString("失敗しました　終了します");
		exit(1);
	}

}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//ウィンドウが破棄されたら呼ばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);//osに対して「もうこのアプリは終わる」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//既定の処理を行う。
}

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
	//ウィンドウクラスの生成＆登録
	DebugOutputFormatString("ウィンドウクラスの生成＆登録\n");

	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//コールバック関数の指定
	w.lpszClassName = ("DX12sample");			//アプリケーションクラス名(適当でよい)
	w.hInstance = GetModuleHandle(nullptr);		//ハンドルの取得
	RegisterClassEx(&w);						//アプリケーションクラス(ウィンドウクラスの指定をosに伝える)

	RECT wrc = { 0,0,window_width,window_height };	//ウィンドウサイズを決める
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//関数を使ってウィンドウサイズを補正する

//ウィンドウオブジェクトの生成
DebugOutputFormatString("ウィンドウオブジェクトの生成\n");
	
	HWND hwnd = CreateWindow(w.lpszClassName,	//クラス名指定
		("DX12テスト"),							//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,					//タイ	トルバーと境界線があるウィンドウ
		CW_USEDEFAULT,							//表示x座標はosにお任せ
		CW_USEDEFAULT,							//表示y座標はosにお任せ
		wrc.right - wrc.left,					//ウィンドウ幅
		wrc.bottom - wrc.top,					//ウィンドウ高
		nullptr,								//親ウィンドウハンドル
		nullptr,								//メニューハンドル
		w.hInstance,							//呼び出しアプリケーションハンドル
		nullptr);								//追加パラメータ

//ウィンドウ表示
DebugOutputFormatString("ウィンドウ表示\n");

	ShowWindow(hwnd, SW_SHOW);

//Direct3D DXGIの設定
DebugOutputFormatString("Direct3D DXGIの設定\n");
	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgifactory = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

//Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL featureLevel;

	for (auto lv : levels) {
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			DebugOutputFormatString( "デバイスが生成されました\n");
			break;//生成可能なバージョンが見つかったらループを打ち切り
		}	
	}
	if (_dev == nullptr) {
		DebugOutputFormatString("強制終了します\n");
		exit(1);
	}
//DXGIFactoryオブジェクトの生成
DebugOutputFormatString("DXGIFactoryオブジェクトの生成\n");
	re_check(CreateDXGIFactory1(IID_PPV_ARGS(&_dxgifactory)));

//コマンドリストとコマンドアロケータ
DebugOutputFormatString("コマンドリストとコマンドアロケータ作成\n");
	ID3D12CommandAllocator*  _cmdAllocator = nullptr;	
	ID3D12GraphicsCommandList* _cmdList = nullptr;
//コマンドアロケータの作成
DebugOutputFormatString("コマンドアロケータの作成\n");
	re_check(_dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&_cmdAllocator)));

//コマンドリストの作成
DebugOutputFormatString("コマンドリストの作成\n");
	re_check(_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
	_cmdAllocator, nullptr,
	IID_PPV_ARGS(&_cmdList)));

//コマンドキューの作成
DebugOutputFormatString("コマンドキューの作成\n");
	ID3D12CommandQueue* _cmdQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;					//タイムアウトなし
	cmdQueueDesc.NodeMask = 0;											//アダプタを一つしか使わないときは0でよい
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;		//プライオリティは特に指定なし
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;					//コマンドリストと合わせる

	re_check(_dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue)));
//スワップチェーンの生成
DebugOutputFormatString("スワップチェーンの生成\n");

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;				//バックバッファーは伸びチジミ可能
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//フリップ後は速やかに破棄
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;		//特に指定なし

	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//ウィンドウフルスクリーン切替可能
	re_check(_dxgifactory->CreateSwapChainForHwnd(
		_cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&_swapchain));
//ディスクリプタヒープの作成
DebugOutputFormatString("ディスクリプタヒープの作成（RTV用）\n");
	ID3D12DescriptorHeap* rtvHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;			//レンダーターゲットビューにするので
	heapDesc.NodeMask = 0;									//GPUが複数ある時の設定なので1つだけ使用する想定なので0
	heapDesc.NumDescriptors = 2;							//裏表の2つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		//特に指定なし　シェーダ側から参照する必要があるかの指定

	re_check(_dev->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&rtvHeap)));

//スワップチェーンのメモリとディスクリプタを紐づける
DebugOutputFormatString("スワップチェーンのメモリとディスクリプタを紐づける\n");
	
D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();			//RTVheapのハンドルを取得する
	std::vector<ID3D12Resource*> _backBuffers(heapDesc.NumDescriptors);
	for (int idx = 0; idx < heapDesc.NumDescriptors; ++idx) {

		re_check(_swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx])));				//スワップチェインのバックバッファーを取得する
		
		_dev->CreateRenderTargetView(_backBuffers[idx], nullptr, handle);					//レンダーターゲットビューを作成する(スワップチェインのバックバッファーを登録する)



		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	MSG msg = {};
	
//メインループ開始
	DebugOutputFormatString("メインループ開始\n");

	while(true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//アプリケーションが終わるときにmessagageがWM_QUITになる
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

//もうクラスは使わないので登録解除する
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}