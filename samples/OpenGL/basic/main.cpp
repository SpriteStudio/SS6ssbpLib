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

void userDataCallback(ss::Player* player, const ss::UserData* data);
void playEndCallback(ss::Player* player);

// SSプレイヤー
ss::Player *ssplayer;
ss::ResourceManager *resman;

//アプリケーションでの入力操作用
bool nextanime = false;			//次のアニメを再生する
bool forwardanime = false;		//前のアニメを再生する
bool pauseanime = false;
int playindex = 0;				//現在再生しているアニメのインデックス
int playerstate = 0;
std::vector<std::string> animename;	//アニメーション名のリスト


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
	case 122:	//z
		nextanime = true;
		break;
	case 120:	//x
		forwardanime = true;
		break;
	case 99:	//c
		pauseanime = true;
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
	if (wait > 16)
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
//	ssplayer->play("character_template_2head/jump_air");				 // アニメーション名を指定(ssae名/アニメーション名も可能、詳しくは後述)


	//表示位置を設定
	ssplayer->setPosition(WIDTH / 2, HEIGHT / 2);
	ssplayer->setScale(0.5f, 0.5f);

	//ユーザーデータコールバックを設定
	ssplayer->setUserDataCallback(userDataCallback);

	//アニメーション終了コールバックを設定
	ssplayer->setPlayEndCallback(playEndCallback);

	//ssbpに含まれているアニメーション名のリストを取得する
	animename = resman->getAnimeName("character_template1");
	playindex = 0;				//現在再生しているアニメのインデックス
}

//アプリケーション更新
void update(float dt)
{
	//プレイヤーの更新、引数は前回の更新処理から経過した時間
	ssplayer->update(dt);

	if (nextanime == true)
	{
		playindex++;
		if (playindex >= animename.size())
		{
			playindex = 0;
		}
		std::string name = animename.at(playindex);
		ssplayer->play(name);
		nextanime = false;
	}
	if (forwardanime == true)
	{
		playindex--;
		if ( playindex < 0 )
		{
			playindex = animename.size() - 1;
		}
		std::string name = animename.at(playindex);
		ssplayer->play(name);
		forwardanime = false;
	}
	if (pauseanime == true)
	{
		if (playerstate == 0)
		{
			ssplayer->animePause();
			playerstate = 1;
		}
		else
		{
			ssplayer->animeResume();
			playerstate = 0;
		}
		pauseanime = false;
	}
}

//ユーザーデータコールバック
void userDataCallback(ss::Player* player, const ss::UserData* data)
{
	//再生したフレームにユーザーデータが設定されている場合呼び出されます。
	//プレイヤーを判定する場合、ゲーム側で管理しているss::Playerのアドレスと比較して判定してください。
	/*
	//コールバック内でパーツのステータスを取得したい場合は、この時点ではアニメが更新されていないため、
	//getPartState　に　data->frameNo　でフレーム数を指定して取得してください。
	ss::ResluteState result;
	//再生しているモーションに含まれるパーツ名「collision」のステータスを取得します。
	ssplayer->getPartState(result, "collision", data->frameNo);
	*/

}

//アニメーション終了コールバック
void playEndCallback(ss::Player* player)
{
	//再生したアニメーションが終了した段階で呼び出されます。
	//プレイヤーを判定する場合、ゲーム側で管理しているss::Playerのアドレスと比較して判定してください。
	//player->getPlayAnimeName();
	//を使用する事で再生しているアニメーション名を取得する事もできます。

	//ループ回数分再生した後に呼び出される点に注意してください。
	//無限ループで再生している場合はコールバックが発生しません。

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

