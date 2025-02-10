#include "CaptureScreen.h"
#include "PreOpenCVHeaders.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include "PostOpenCVHeaders.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include <Windows.h>

using namespace cv;

#pragma region capture screen
// Função para capturar a tela inteira incluindo o mouse
cv::Mat getFullScreenWithCursor(bool includeMouse)
{
    HDC hdc = GetDC(NULL); // Obtém o DC da tela inteira
    HDC hdcMem = CreateCompatibleDC(hdc);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    HBITMAP hbmScreen = CreateCompatibleBitmap(hdc, screenWidth, screenHeight);
    SelectObject(hdcMem, hbmScreen);

    // Captura a tela inteira
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdc, 0, 0, SRCCOPY);

    if (includeMouse) {
        // Captura o cursor do mouse
        CURSORINFO cursorInfo = { 0 };
        cursorInfo.cbSize = sizeof(CURSORINFO);
        if (GetCursorInfo(&cursorInfo))
        {
            if (cursorInfo.flags == CURSOR_SHOWING)
            {
                ICONINFO iconInfo = { 0 };
                if (GetIconInfo(cursorInfo.hCursor, &iconInfo))
                {
                    int x = cursorInfo.ptScreenPos.x - iconInfo.xHotspot;
                    int y = cursorInfo.ptScreenPos.y - iconInfo.yHotspot;
                    DrawIcon(hdcMem, x, y, cursorInfo.hCursor);
                    DeleteObject(iconInfo.hbmMask);
                    DeleteObject(iconInfo.hbmColor);
                }
            }
        }
    }

    BITMAP bmpScreen;
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = -bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    cv::Mat mat(bmpScreen.bmHeight, bmpScreen.bmWidth, CV_8UC4);
    GetDIBits(hdcMem, hbmScreen, 0, (UINT)bmpScreen.bmHeight, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    DeleteObject(hbmScreen);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);

    return mat;
}

// Função para capturar a janela incluindo o cursor do mouse
cv::Mat getWindowWithCursor(HWND hwnd, bool includeMouse)
{
    HDC hwindowDC, hwindowCompatibleDC;
    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    cv::Mat src;
    BITMAPINFOHEADER bi;

    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    RECT windowsize;
    GetClientRect(hwnd, &windowsize);
    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;
    height = windowsize.bottom;
    width = windowsize.right;

    src.create(height, width, CV_8UC4);

    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    SelectObject(hwindowCompatibleDC, hbwindow);
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY);

    if (includeMouse)
    {
        CURSORINFO cursorInfo = { 0 };
        cursorInfo.cbSize = sizeof(CURSORINFO);
        if (GetCursorInfo(&cursorInfo))
        {
            if (cursorInfo.flags == CURSOR_SHOWING)
            {
                ICONINFO iconInfo = { 0 };
                if (GetIconInfo(cursorInfo.hCursor, &iconInfo))
                {
                    ScreenToClient(hwnd, &cursorInfo.ptScreenPos);
                    int x = cursorInfo.ptScreenPos.x - iconInfo.xHotspot;
                    int y = cursorInfo.ptScreenPos.y - iconInfo.yHotspot;
                    DrawIcon(hwindowCompatibleDC, x, y, cursorInfo.hCursor);
                    DeleteObject(iconInfo.hbmMask);
                    DeleteObject(iconInfo.hbmColor);
                }
            }
        }
    }

    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);


   // cv::imwrite("C:\\teste\\converted_image.png", src);

    return src;
}

#pragma endregion

#pragma region Multiplos Monitores

//********************************************************************************************************************


// Função de callback para enumeração de monitores
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    std::vector<HMONITOR>* monitors = reinterpret_cast<std::vector<HMONITOR>*>(dwData);
    monitors->push_back(hMonitor);
    return TRUE;
}

// Função para capturar a tela de um monitor específico, incluindo o cursor
cv::Mat getMonitorScreenWithCursor(int monitorIndex, bool includeMouse)
{
    // Enumera os monitores
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors));
    if (monitorIndex >= monitors.size())
    {
        std::cerr << "Monitor index out of range!" << std::endl;
        return cv::Mat();
    }

    // Obtém o HDC do monitor desejado
    HMONITOR hMonitor = monitors[monitorIndex];
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    if (!GetMonitorInfo(hMonitor, &monitorInfo))
    {
        std::cerr << "GetMonitorInfo failed!" << std::endl;
        return cv::Mat();
    }

    HDC hdcMonitor = CreateDC(NULL, monitorInfo.szDevice, monitorInfo.szDevice, NULL);

    HDC hdcMem = CreateCompatibleDC(hdcMonitor);
    int screenWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    int screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcMonitor, screenWidth, screenHeight);
    SelectObject(hdcMem, hbmScreen);

    // Captura a tela do monitor
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcMonitor, 0, 0, SRCCOPY);

    if (includeMouse) {
        // Captura o cursor do mouse
        CURSORINFO cursorInfo = { 0 };
        cursorInfo.cbSize = sizeof(CURSORINFO);
        if (GetCursorInfo(&cursorInfo))
        {
            if (cursorInfo.flags == CURSOR_SHOWING)
            {
                ICONINFO iconInfo = { 0 };
                if (GetIconInfo(cursorInfo.hCursor, &iconInfo))
                {
                    // Posição do cursor
                    int x = cursorInfo.ptScreenPos.x - monitorInfo.rcMonitor.left - iconInfo.xHotspot;
                    int y = cursorInfo.ptScreenPos.y - monitorInfo.rcMonitor.top - iconInfo.yHotspot;

                    // Desenha o cursor
                    DrawIconEx(hdcMem, x, y, cursorInfo.hCursor, 0, 0, 0, NULL, DI_NORMAL);
                    DeleteObject(iconInfo.hbmMask);
                    DeleteObject(iconInfo.hbmColor);
                }
            }
        }
    }

    BITMAP bmpScreen;
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = -bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    cv::Mat mat(bmpScreen.bmHeight, bmpScreen.bmWidth, CV_8UC4);
    GetDIBits(hdcMem, hbmScreen, 0, (UINT)bmpScreen.bmHeight, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Limpeza de objetos GDI
    DeleteObject(hbmScreen);
    DeleteDC(hdcMem);
    DeleteDC(hdcMonitor);


    return mat;
}

#pragma endregion



// Salvar a imagem capturada para depuração
//cv::imwrite("C:\\teste\\captured_image.png", mat);

ACaptureScreen::ACaptureScreen()
{
    PrimaryActorTick.bCanEverTick = true;

    CaptureMode = ECaptureMode::Window;
}

void ACaptureScreen::BeginPlay()
{
    Super::BeginPlay();

}

void ACaptureScreen::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	//if (CaptureOn)	
    //    UpdateRenderTarget();
}

LPCWSTR FindWindowPartialTitle(LPCWSTR partial_title)
{
    static wchar_t window_title[256]; // Static para que a string não saia do escopo

    HWND hwnd = GetTopWindow(NULL); // Começar a partir da janela superior

    while (hwnd != NULL)
    {
        if (IsWindowVisible(hwnd)) // Verifique se a janela está visível
        {
            GetWindowText(hwnd, window_title, sizeof(window_title) / sizeof(wchar_t));

            if (wcsstr(window_title, partial_title) != NULL) // Use wcsstr para encontrar a substring
            {
              //  UE_LOG(LogTemp, Warning, TEXT("Title: %s"), window_title);

                return window_title; // Retornar o título completo da janela
            }
        }

        hwnd = GetNextWindow(hwnd, GW_HWNDNEXT); // Próxima janela
    }

    return NULL; // Nenhuma janela encontrada, retornar NULL
}


void ACaptureScreen::UpdateRenderTarget()
{
    if (RenderTarget == nullptr)
    {
        return;
    }

    LPCWSTR partial_title = *WindowTitle;
    LPCWSTR window_title = FindWindowPartialTitle(partial_title);

    // Capturar a janela com o cursor
    HWND hwnd = FindWindow(NULL, window_title);
    //Mat mat = getWindowWithCursor(hwnd, true);
	Mat mat;

    if (CaptureMode == ECaptureMode::Window)
    {
		mat = getWindowWithCursor(hwnd, true); // Captura a janela incluindo o cursor do mouse
    }
    else if (CaptureMode == ECaptureMode::FullScreen)
    {
		mat = getFullScreenWithCursor(true); // Captura a tela inteira incluindo o cursor do mouse
    }
    else if (CaptureMode == ECaptureMode::Monitor)
    {
         mat = getMonitorScreenWithCursor(monitorIndex, true); // Captura o monitor incluindo o cursor do mouse
    }
    else
    {
		mat = getWindowWithCursor(hwnd, true); // Captura a janela incluindo o cursor do mouse
    }

    if (mat.empty())
    {
        return;
    }

    // Verificar as dimensões do Render Target
    FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    uint32 TextureWidth = renderTargetResource->GetRenderTargetTexture()->GetSizeX();
    uint32 TextureHeight = renderTargetResource->GetRenderTargetTexture()->GetSizeY();

    //UE_LOG(LogTemp, Warning, TEXT("RenderTarget Size: %d x %d"), TextureWidth, TextureHeight);
    
    
   // UE_LOG(LogTemp, Warning, TEXT("RenderTarget Size: %d x %d"), mat.size().width, mat.size().height);

	if (AutoSetRenderTargetSize)
	{
		RenderTarget->ResizeTarget(mat.size().width, mat.size().height);
    }
    else 
    {
        // Redimensionar a imagem capturada para corresponder ao tamanho do Render Target
        cv::resize(mat, mat, cv::Size(static_cast<int32>(TextureWidth), static_cast<int32>(TextureHeight)));
    }

    // Converter a imagem OpenCV de BGRA para RGBA
 /*   cv::Mat matRGBA;
    cv::cvtColor(mat, matRGBA, cv::COLOR_BGRA2RGBA);

     // Converter a imagem OpenCV para um array de FLinearColor
    TArray<FLinearColor> textureData;
    textureData.SetNum(matRGBA.rows * matRGBA.cols);
    for (int y = 0; y < matRGBA.rows; y++)
    {
        for (int x = 0; x < matRGBA.cols; x++)
        {
            Vec4b& rgba = matRGBA.at<Vec4b>(y, x);
            int index = y * matRGBA.cols + x;
            textureData[index] = FLinearColor(
                rgba[0] / 255.0f, // R
                rgba[1] / 255.0f, // G
                rgba[2] / 255.0f, // B
                rgba[3] / 255.0f  // A
            );
        }
    }*/


    // Converter a imagem OpenCV de BGRA para RGBA
    cv::Mat matRGBA;
    cv::cvtColor(mat, matRGBA, cv::COLOR_BGRA2RGBA);

   // cv::imwrite("C:\\teste\\converted_image.png", matRGBA);

    // Converter a imagem OpenCV para um array de FLinearColor
    TArray<FLinearColor> textureData;
    textureData.SetNum(matRGBA.rows * matRGBA.cols);

    for (int y = 0; y < matRGBA.rows; y++)
    {
        for (int x = 0; x < matRGBA.cols; x++)
        {
            Vec4b& rgba = matRGBA.at<Vec4b>(y, x);
            int index = y * matRGBA.cols + x;

            // Aplicar correção gamma para sRGB -> Linear
            textureData[index] = FLinearColor(
                sRGBToLinear(rgba[0]), // R
                sRGBToLinear(rgba[1]), // G
                sRGBToLinear(rgba[2]), // B
                rgba[3] / 255.0f       // A (permanece inalterado, pois é linear)
            );
        }
    }

    // Converter o array de FLinearColor para uint8
    TArray<uint8> rawData;
    rawData.SetNum(textureData.Num() * sizeof(FLinearColor));
    FMemory::Memcpy(rawData.GetData(), textureData.GetData(), textureData.Num() * sizeof(FLinearColor));

    // Ajustar a região de atualização
    FUpdateTextureRegion2D updateRegion(0, 0, 0, 0, matRGBA.cols, matRGBA.rows);

    // Copiar os dados para o Render Target usando RHI
    ENQUEUE_RENDER_COMMAND(CaptureFrame)(
        [renderTargetResource, rawData, updateRegion](FRHICommandListImmediate& RHICmdList)
        {
            RHICmdList.UpdateTexture2D(
                renderTargetResource->GetRenderTargetTexture(), 0, updateRegion,
                updateRegion.Width * sizeof(FLinearColor), rawData.GetData()
            );
        }
        );
}

void ACaptureScreen::SimulateMouseClick(FVector2D position)
{
    // Posiciona o mouse em uma posição específica na tela
    SetCursorPos(position.X, position.Y); // Altere as coordenadas conforme necessário

    // Configura o evento de clique do mouse
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN; // Pressiona o botão esquerdo
    SendInput(1, &input, sizeof(INPUT));
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP; // Solta o botão esquerdo
    SendInput(1, &input, sizeof(INPUT));
}

// Implementação da função que retorna a lista de títulos das janelas abertas
TArray<FString> ACaptureScreen::ListOpenWindowTitles()
{
    TArray<FString> WindowTitles; // Array para armazenar os títulos das janelas

    static wchar_t window_title[256]; // Static para que a string não saia do escopo
    HWND hwnd = GetTopWindow(NULL); // Começar a partir da janela superior
    while (hwnd != NULL)
    {
        GetWindowText(hwnd, window_title, sizeof(window_title) / sizeof(wchar_t));
        std::wstring window_title_str(window_title);

        // Verifica se a janela é visível e válida
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

        if (IsWindowVisible(hwnd) && window_title_str.length() > 0 && window_title_str != L"Default IME" && window_title_str != L"MSCTFIME UI" && (style & WS_CAPTION) && !(exStyle & WS_EX_TOOLWINDOW))
        {
            WindowTitles.Add(window_title);
        }
        hwnd = GetNextWindow(hwnd, GW_HWNDNEXT); // Próxima janela                
    }

    return WindowTitles; // Retorna o array com os títulos das janelas
}




//********************************************************************************************************************

// Função auxiliar para conversão de sRGB para Linear
float ACaptureScreen::sRGBToLinear(float value)
{
    value /= 255.0f;
    if (value <= 0.04045f)
    {
        return value / 12.92f;
    }
    else
    {
        return std::pow((value + 0.055f) / 1.055f, 2.4f);
    }
}

