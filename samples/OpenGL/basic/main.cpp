#include <windows.h>
#include <stdio.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>

#include "./SSPlayer/SS6Player.h"

//画面サイズ
#define WIDTH (1280)
#define HEIGHT (720)

//FPS制御用
int nowtime = 0;	//経過時間
int drawtime = 0;	//前回の時間

// SSプレイヤー
ss::Player *ssplayer;
ss::ResourceManager *resman;

//glutのコールバック関数
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void idle(void);
void disp(void);

//アプリケーションの制御
void Init();
void update(float dt);
void relese(void);
void draw(void);


//アプリケーションのメイン関数関数
int main(int argc, char ** argv) 
{
	//ライブラリ初期化
	glutInit(&argc, argv);				//GLUTの初期化
	//ウィンドウ作成
	glutInitWindowPosition(100, 50);	//ウィンドウ位置設定
	glutInitWindowSize(WIDTH, HEIGHT);	//ウィンドウサイズ設定
	glutInitDisplayMode(GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH | GLUT_DOUBLE);	//使用するバッファを設定
	glutCreateWindow("Sprite Studio SS6ssbpLib Sample");		//ウィンドウタイトル

	GLenum err;
	err = glewInit();					//GLEWの初期化
	if (err != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << '\n';
		return 0;
	}

	glClearColor(0.0, 0.0, 0.2, 1.0);		//背景色

	//割り込み設定	
	glutIdleFunc(idle);			//アイドルコールバック設定
	glutDisplayFunc(disp);		//表示コールバック設定
	glutKeyboardFunc(keyboard);	//キーボード入力コールバック設定
	glutMouseFunc(mouse);		//マウス入力コールバック設定

	Init();

	glutMainLoop();				//メインループ
	return 0;
}

//キーボード入力コールバック
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:	//esc
		relese();					//アプリケーション終了
		exit(0);
		break;
	default:
		break;
	}
}

//マウス入力コールバック
void mouse(int button, int state, int x, int y)
{
}

//アイドルコールバック
void idle(void) 
{
	//FPSの設定
	nowtime = glutGet(GLUT_ELAPSED_TIME);//経過時間を取得
	int wait = nowtime - drawtime;
	if (wait >= 16)
	{
		update((float)wait / 1000.0f);
		glutPostRedisplay();
		drawtime = nowtime;
	}
}

//描画コールバック
void disp(void)
{
	//レンダリング開始時の初期化
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	//フレームバッファのクリア
	glDisable(GL_STENCIL_TEST);							//ステンシル無効にする
	glEnable(GL_DEPTH_TEST);							//深度バッファを有効にする
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	//フレームバッファへ各色の書き込みを設定

	draw();

	//終了処理
	glDisable(GL_DEPTH_TEST);	//深度テストを無効にする
	glDisable(GL_ALPHA_TEST);	//アルファテスト無効にする
	glDisable(GL_TEXTURE_2D);	//テクスチャ無効
	glDisable(GL_BLEND);		//ブレンドを無効にする

	glutSwapBuffers();
}

//アプリケーション初期化処理
void Init()
{
	/**********************************************************************************

	SpriteStudioアニメーション表示のサンプルコード
	Visual Studio Community 2017で動作を確認しています。
	WindowsSDK(デスクトップC++ x86およびx64用のWindows10 SDK)をインストールする必要があります
	プロジェクトのNuGetでglutを検索しnupengl.coreを追加してください。

	ssbpとpngがあれば再生する事ができますが、Resourcesフォルダにsspjも含まれています。

	**********************************************************************************/

	//プレイヤーを使用する前の初期化処理
	//この処理はアプリケーションの初期化で１度だけ行ってください。
	ss::SSPlatformInit();
	//Y方向の設定とウィンドウサイズ設定を行います
	ss::SSSetPlusDirection(ss::PLUS_UP, WIDTH, HEIGHT);
	//リソースマネージャの作成
	resman = ss::ResourceManager::getInstance();
	//プレイヤーを使用する前の初期化処理ここまで


	//プレイヤーの作成
	ssplayer = ss::Player::create();

	//アニメデータをリソースに追加

	//それぞれのプラットフォームに合わせたパスへ変更してください。
	resman->addData("Resources/character_template_comipo/character_template1.ssbp");
	//プレイヤーにリソースを割り当て
	ssplayer->setData("character_template1");        // ssbpファイル名（拡張子不要）
	//再生するモーションを設定
	ssplayer->play("character_template_3head/stance");				 // アニメーション名を指定(ssae名/アニメーション名も可能、詳しくは後述)


	//表示位置を設定
	ssplayer->setPosition(WIDTH / 2, HEIGHT / 2);
	ssplayer->setScale(0.5f, 0.5f);
}

//アプリケーション更新
void update(float dt)
{
	//プレイヤーの更新、引数は前回の更新処理から経過した時間
	ssplayer->update(dt);
}

//アプリケーション描画
void draw(void)
{
	//プレイヤーの描画
	ssplayer->draw();
}

//アプリケーション終了処理
void relese(void)
{
	//SSPlayerの削除
	delete (ssplayer);
	delete (resman);
	ss::SSPlatformRelese( );
}

