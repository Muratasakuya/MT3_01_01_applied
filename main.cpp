#include <Novice.h>
#include "MyMath.h"
#include <ImGui.h>

const char kWindowTitle[] = "LC1B_28_ムラタ_サクヤ_MT3_01_01_basic";

static const int kColumnWidth = 60;

/// <summary>
/// 三次元ベクトルの数値表示
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="vector"></param>
/// <param name="label"></param>
void Vector3ScreenPrintf(int x, int y, const Vec3f& vector, const char* label) {

	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

/// <summary>
/// 透視投影行列
/// </summary>
/// <param name="fovY"></param>
/// <param name="aspectRatio"></param>
/// <param name="nearClip"></param>
/// <param name="farClip"></param>
/// <returns></returns>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {

	Matrix4x4 matrix = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = 0.0f;
		}
	}

	matrix.m[0][0] = 1.0f / (aspectRatio * std::tanf(fovY / 2.0f));
	matrix.m[1][1] = 1.0f / std::tanf(fovY / 2.0f);
	matrix.m[2][2] = farClip / (farClip - nearClip);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][2] = -farClip * (nearClip / (farClip - nearClip));

	return matrix;
}

/// <summary>
/// 正射影行列
/// </summary>
/// <param name="left"></param>
/// <param name="top"></param>
/// <param name="right"></param>
/// <param name="bottom"></param>
/// <param name="nearClip"></param>
/// <param name="farClip"></param>
/// <returns></returns>
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip, float zoom) {

	Matrix4x4 matrix = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = 0.0f;
		}
	}

	matrix.m[0][0] = 2.0f / (right - left) * zoom;
	matrix.m[1][1] = 2.0f / (top - bottom) * zoom;
	matrix.m[2][2] = 1.0f / (farClip - nearClip);
	matrix.m[3][0] = (left + right) / (left - right);
	matrix.m[3][1] = (top + bottom) / (bottom - top);
	matrix.m[3][2] = nearClip / (nearClip - farClip);
	matrix.m[3][3] = 1.0f;

	return matrix;
}

/// <summary>
/// ビューポート変換行列
/// </summary>
/// <param name="left"></param>
/// <param name="top"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="minDepth"></param>
/// <param name="maxSepth"></param>
/// <returns></returns>
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {

	Matrix4x4 matrix = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = 0.0f;
		}
	}

	matrix.m[0][0] = width / 2.0f;
	matrix.m[1][1] = -height / 2.0f;
	matrix.m[2][2] = maxDepth - minDepth;
	matrix.m[3][0] = left + width / 2.0f;
	matrix.m[3][1] = top + height / 2.0f;
	matrix.m[3][2] = minDepth;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

#pragma warning(push)
#pragma warning(disable:26495)
/// <summary>
/// 表示する三角形の構造体
/// </summary>
struct Triangle {

	Matrix4x4 worldMatrix;
	Matrix4x4 worldViewProjectionMatrix;

	Vec3f kLocalVertices[3];
	Vec3f screenVertices[3];
	Vec3f ndcVertex;

	Vec3f rotate;
	Vec3f translate;

	float speed;
};

/// <summary>
/// カメラの構造体
/// </summary>
struct Camera {

	Matrix4x4 matrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 orthoMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 viewportMatrix;

	Vec3f pos;

	float zoom;
};
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:28251)
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma warning(pop)

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// 変数宣言
	// 三角形描画で使う変数
	Triangle triangle;

	// 座標
	triangle.translate = { 0.0f,0.0f,200.0f };
	triangle.rotate = { 0.0f,0.0f,0.0f };
	// 各頂点
	triangle.kLocalVertices[0] = { 0.0f, 25.0f, 0.0f };
	triangle.kLocalVertices[1] = { 25.0f, 0.0f, 0.0f };
	triangle.kLocalVertices[2] = { -25.0f, 0.0f, 0.0f };
	// 値を入れて変換
	triangle.worldMatrix =
		MakeAffineMatrix({ 1.0f,1.0f,1.0f }, triangle.rotate, triangle.translate);
	// 動く速度
	triangle.speed = 5.0f;

	// カメラで使う変数
	Camera camera;

	// カメラ座標
	camera.pos = { 0.0f,0.0f,0.0f };
	camera.zoom = 1.0f;

	camera.matrix =
		MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, camera.pos);

	camera.viewMatrix = Inverse(camera.matrix);

	camera.projectionMatrix =
		MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);

	triangle.worldViewProjectionMatrix =
		Multiply(triangle.worldMatrix, Multiply(camera.viewMatrix, camera.projectionMatrix));

	camera.viewportMatrix =
		MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

	for (int i = 0; i < 3; i++) {

		triangle.ndcVertex =
			Transform(triangle.kLocalVertices[i], triangle.worldViewProjectionMatrix);
		triangle.screenVertices[i] = Transform(triangle.ndcVertex, camera.viewportMatrix);
	}

	// 背面カリングを行うのに使う変数
	Vec3f edge1 = triangle.kLocalVertices[1] - triangle.kLocalVertices[0];
	Vec3f edge2 = triangle.kLocalVertices[2] - triangle.kLocalVertices[0];

	Vec3f cross = Cross(edge1, edge2);

	Vec3f viewDirection = camera.pos - triangle.kLocalVertices[0];

	float dot = Dot(cross, viewDirection);

	bool isDisplay = NULL;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		ImGui::Begin("renderingPipeline");

		ImGui::SliderFloat("camera.zoom", &camera.zoom, 0.0f, 10.0f);
		ImGui::SliderFloat2("cameraPos", &camera.pos.x, -1280.0f, 1280.0f);
		ImGui::SliderFloat3(
			"kLocalVertices[0]", &triangle.kLocalVertices[0].x, -1280.0f, 1280.0f);
		ImGui::SliderFloat3(
			"kLocalVertices[1]", &triangle.kLocalVertices[1].x, -1280.0f, 1280.0f);
		ImGui::SliderFloat3(
			"kLocalVertices[2]", &triangle.kLocalVertices[2].x, -1280.0f, 1280.0f);

		ImGui::End();

		/************************************************************************
			移動処理	*/

		if (keys[DIK_W]) {

			triangle.translate.y += triangle.speed;
		} else if (keys[DIK_S]) {

			triangle.translate.y -= triangle.speed;
		}

		if (keys[DIK_A]) {

			triangle.translate.x -= triangle.speed;
		} else if (keys[DIK_D]) {

			triangle.translate.x += triangle.speed;
		}

		if (keys[DIK_Q]) {

			triangle.translate.z -= triangle.speed;
		} else if (keys[DIK_E]) {

			triangle.translate.z += triangle.speed;
		}

		/************************************************************************/

		// y軸回転
		triangle.rotate.y += static_cast<float>(M_PI / 120.0f);

		triangle.worldMatrix =
			MakeAffineMatrix({ 1.0f,1.0f,1.0f }, triangle.rotate, triangle.translate);

		camera.matrix =
			MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, camera.pos);
		camera.viewMatrix = Inverse(camera.matrix);

		triangle.worldViewProjectionMatrix =
			Multiply(triangle.worldMatrix, Multiply(camera.viewMatrix, camera.projectionMatrix));

		for (int i = 0; i < 3; i++) {

			// NDC変換
			triangle.ndcVertex =
				Transform(triangle.kLocalVertices[i], triangle.worldViewProjectionMatrix);
			// スクリーン空間に変換
			triangle.screenVertices[i] = Transform(triangle.ndcVertex, camera.viewportMatrix);
		}

		// 背面カリングの処理
		edge1 = triangle.screenVertices[1] - triangle.screenVertices[0];
		edge2 = triangle.screenVertices[2] - triangle.screenVertices[0];

		cross = Cross(edge1, edge2);

		viewDirection = camera.pos - triangle.screenVertices[0];

		dot = Dot(cross, viewDirection);

		// dotが正の場合は表側、負の場合は裏側
		if (dot > 0.0f) {
			isDisplay = true;
		} else {
			isDisplay = false;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// 表側なら描画する
		if (isDisplay) {
			// 三角形の描画
			Novice::DrawTriangle(
				static_cast<int>(triangle.screenVertices[0].x), static_cast<int>(triangle.screenVertices[0].y),
				static_cast<int>(triangle.screenVertices[1].x), static_cast<int>(triangle.screenVertices[1].y),
				static_cast<int>(triangle.screenVertices[2].x), static_cast<int>(triangle.screenVertices[2].y),
				RED, kFillModeSolid
			);
		}

		// 三角形のデバッグ表示
		Vector3ScreenPrintf(24, 48, triangle.translate, " : translate");
		Novice::ScreenPrintf(24, 72, "dot = %4.1f", dot);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
