#include "Shader.h"
#include "Buffer.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <d3d11.h>
#include <dxgi.h>

#include <glm/glm.hpp>

#include <iostream>

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// GLFW Process input
void processInput(GLFWwindow* window);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// Forward declarations
void InitD3D(HWND hWnd);
void CleanUpDirectX();
void InitGraphics(Shader* shader);

// Global DirectX variables
IDXGISwapChain* swapchain;             // the pointer to the swap chain interface
ID3D11Device* dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext* devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView* backbuffer;    // the  pointer to our BackBuffer

ID3D11InputLayout* pLayout;    // global

const char* vertexShaderSource = R"(
struct VS_INPUT
{
    float3 Pos : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = float4(input.Pos, 1.0f);
    return output;
}
)";

const char* pixelShaderSource = R"(
cbuffer ConstantBuffer : register(b0)
{
    float4 color;
};
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_Target
{
    return color;
}
)";

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DirectX with GLFW", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Get the HWND from GLFW window

    HWND hwnd = glfwGetWin32Window(window);

    // Initialize DirectX
    InitD3D(hwnd);

    // Initialize graphics (shaders, buffers)
    Shader* shader = new Shader(vertexShaderSource, pixelShaderSource, dev);
    InitGraphics(shader);

    // create a triangle using the VERTEX struct
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    VertexBuffer* vertexBuffer = new VertexBuffer(vertices, dev);

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

    IndexBuffer* indexBuffer = new IndexBuffer(indices, dev);
    

    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);
        glfwPollEvents();

        // render
        // ------
        // Clear the screen
        float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        // clear the back buffer to a deep blue
        devcon->ClearRenderTargetView(backbuffer, clearColor);

        float time = static_cast<float>(glfwGetTime());

        // Update constant buffer with current time
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        devcon->Map(shader->GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        Shader::ConstantBuffer* cb = (Shader::ConstantBuffer*)mappedResource.pData;
        float timeValue = glfwGetTime();
        float redValue = sin(timeValue) / 2.0f + 0.5f;
        cb->color = { redValue, 0.0f, 0.0f, 1.0f };
        devcon->Unmap(shader->GetConstantBuffer(), 0);

        // do 3D rendering on the back buffer here
        // Set vertex buffer
        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        ID3D11Buffer* pVBuffer = vertexBuffer->GetVertexBuffer();
        devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
        devcon->IASetIndexBuffer(indexBuffer->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

        // Set primitive topology
        devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Render the triangle
        devcon->VSSetShader(shader->GetVertexShader(), nullptr, 0);
        devcon->PSSetShader(shader->GetPixelShader(), nullptr, 0);

        // Set constant buffer
        ID3D11Buffer* pCBuffer = shader->GetConstantBuffer();
        devcon->PSSetConstantBuffers(0, 1, &pCBuffer);
        devcon->DrawIndexed(indexBuffer->GetIndicesSize(), 0, 0);

        // switch the back buffer and the front buffer
        swapchain->Present(0, 0);
    }

    // Clean up DirectX
    shader->GetVertexShader()->Release();
    shader->GetPixelShader()->Release();
    CleanUpDirectX();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferDesc.Width = SCREEN_WIDTH;                    // set the back buffer width
    scd.BufferDesc.Height = SCREEN_HEIGHT;                  // set the back buffer height
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 1;                               // how many multisamples
    scd.SampleDesc.Quality = 0;                             // multisample quality level
    scd.Windowed = TRUE;                                    // windowed/full-screen mode
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

    // create a device, device context and swap chain using the information in the scd struct
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &dev,
        NULL,
        &devcon);

    if (FAILED(hr)) {
        std::cerr << "Failed to create DirectX device and swap chain" << std::endl;
        exit(-1);
    }

    // get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, NULL);

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    devcon->RSSetViewports(1, &viewport);
}

void InitGraphics(Shader* shader)
{

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    dev->CreateInputLayout(layout, ARRAYSIZE(layout), shader->GetVSBlob()->GetBufferPointer(), shader->GetVSBlob()->GetBufferSize(), &pLayout);

    // Set the input layout
    devcon->IASetInputLayout(pLayout);

    // Release blobs
    shader->GetVSBlob()->Release();
    shader->GetPSBlob()->Release();
}

void CleanUpDirectX() {
    swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

    //if (g_pRenderTargetView) g_pRenderTargetView->Release();
    // close and release all existing COM objects
    backbuffer->Release();
    swapchain->Release();
    dev->Release();
    devcon->Release();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
