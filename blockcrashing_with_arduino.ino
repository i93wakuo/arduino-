#include <VGAX.h>

VGAX vga;

//操作キー
#define START  11
#define LEFT  12
#define RIGHT 10

//フィールドの範囲
#define HEIGHT  50
#define WIDTH 70

//バーの長さ
#define BAR 7

//ブロックの長さ
#define BLOCK 4

//ボールの角度
#define A_LEFT  0
#define A_RIGHT 1
#define A_DOWN  2
#define A_UP  3

//ボールの色
#define BALLCOLOR  1
//その他の色
#define OTHERCOLOR  1

//ボールのスピード　数字が大きければ大きいほど遅い
#define SPEED 100

void setup() {

  pinMode(START, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);

  vga.begin();
  vga.clear(0);
}

void loop() {
  int i, j;
  int bx, by; //ボールの座標
  int px, py; //バーの座標
  char xangle; //ボールのx座標の角度
  char yangle;  //ボールのy座標の角度
  boolean block[HEIGHT / 4][WIDTH / (BLOCK + 1)]; //ブロック


  //初期化
  for (i = 0; i < HEIGHT / 4; i++) {
    for (j = 0; j < WIDTH / (BLOCK + 1); j++) {
      block[i][j] = 1;
    }
  }

  //初めはボールは、左上に進む
  xangle = A_LEFT;
  yangle = A_UP;

  //描画
  vga.clear(0); //画面を黒くする
  //ステージの描画
  px = py = 0;
  for (i = 0; i < HEIGHT / 4; i++, py++) {
    for (j = px = 0; j < WIDTH / (BLOCK + 1); j++) {
      vga.fillrect(j * (BLOCK + 1), py, BLOCK, 1, OTHERCOLOR); //ブロックの描画
    }
    py++;  //ブロックとブロックの間に空白を入れる
  }
  //プレイヤー（バー）の描画
  px = WIDTH / 2;
  py = HEIGHT;
  vga.fillrect(px, py, BAR, 1, OTHERCOLOR); //プレイヤー（バー）
  //ボールの描画
  bx = px;
  by = py - 1;
  vga.fillrect(bx, by, 1, 1, BALLCOLOR); //ボール

  //START ボタンが押されるまで待つ
  while (digitalRead(START) != HIGH) {
    vga.delay(1); //入れずに ; だけにすると、映像出力が不安定になるので入れている
  }

  //ボールが落ちるまで、継続する
  while (by <= HEIGHT) {

    //プレイヤーの操作
    if (digitalRead(LEFT) == HIGH) {  //LEFTキーが押された
      if (px > 0) {  //バーが移動可能か
        vga.fillrect(px, py, BAR, 1, 0); //プレイヤーを消去
        --px;
        vga.fillrect(px, py, BAR, 1, OTHERCOLOR); //プレイヤーを描画
      }
    }
    if (digitalRead(RIGHT) == HIGH) { //RIGHTキーが押された
      if (px + BAR < WIDTH) { //バーが移動可能か
        vga.fillrect(px, py, BAR, 1, 0); //プレイヤーを消去
        ++px;
        vga.fillrect(px, py, BAR, 1, OTHERCOLOR); //プレイヤーを描画
      }
    }

    if (by == py && (bx >= px && bx <= px + BAR) ? 1 : 0) { //ボールがバーに触れたら
      vga.fillrect(bx, by, 1, 1, 0);  //今まであった場所のボールを消す
      yangle = A_UP;  //ボールの角度を上方向へ
      vga.fillrect(bx, by, 1, 1, BALLCOLOR);  //ボールの描画
    }

    if (by < 0) {  //ボールが天井に触れたら
      vga.fillrect(bx, by, 1, 1, 0);  //今まであった場所のボールを消す
      yangle = A_DOWN;  //ボールの角度を下方向へ
      vga.fillrect(bx, by, 1, 1, BALLCOLOR);  //ボールの描画
    }

    if (bx > WIDTH) {  //ボールが右の壁に触れたら
      vga.fillrect(bx, by, 1, 1, 0);  //今まであった場所のボールを消す
      xangle = A_LEFT;
      vga.fillrect(bx, by, 1, 1, BALLCOLOR);  //ボールの描画
    }

    if (bx < 0) {  //ボールが左の壁に触れたら
      vga.fillrect(bx, by, 1, 1, 0);  //今まであった場所のボールを消す
      xangle = A_RIGHT;
      vga.fillrect(bx, by, 1, 1, BALLCOLOR);  //ボールの描画
    }

    //ボールがブロックに触れたか
    for (i = 0; i < HEIGHT / 4; i++) {
      for (j = 0; j < WIDTH / (BLOCK + 1); j++) {
        if ((((bx >= j * (BLOCK + 1)) && (bx <= j * (BLOCK + 1) + BLOCK)) ? 1 : 0) && by == i * 2) { //ボールがブロックに衝突している
          if (block[i][j] == 1) { //かつ、ブロックが消滅していないか
            vga.fillrect(j * (BLOCK + 1), i * 2, BLOCK, 1, 0); //衝突したブロックを消す
            block[i][j] = 0;

            vga.fillrect(bx, by, 1, 1, 0);  //今まであった場所のボールを消す

            switch (yangle) {
              case A_DOWN:
                yangle = A_UP;  //ボールを上方向へ
                break;
              case A_UP:
                yangle = A_DOWN;  //ボールを下方向へ
                break;
            }
            vga.fillrect(bx, by, 1, 1, BALLCOLOR);  //ボールの描画
          }
        }
      }
    }

    //慣性
    vga.fillrect(bx, by, 1, 1, 0);  //ボールを消す
    switch (xangle) { //ボールが右を向いていたら、右のままでいる
      case A_LEFT:
        --bx;
        break;
      case A_RIGHT:
        ++bx;
        break;
    }
    switch (yangle) { //ボールが下を向いていたら、下のままでいる
      case A_DOWN:
        ++by;
        break;
      case A_UP:
        --by;
        break;
    }
    vga.fillrect(bx, by, 1, 1, BALLCOLOR);  //ボールの描画

    vga.delay(SPEED); //スピード

    //ゲームクリアの判定
    for (int count = 0, i = 0; i < HEIGHT / 4; i++) { //count ゲームクリアの判定用
      for (j = 0; j < WIDTH / (BLOCK + 1); j++) {
        if (block[i][j] == 0) {
          count++;
          if (count == HEIGHT / 4 * WIDTH / (BLOCK + 1)) {
            //ゲームクリア！
            vga.clear(0);

            //クリアメッセージ
            for (i = 0; i < 10; i++) {
              vga.fillrect(0, i, 2, 1, BALLCOLOR);
            }
            vga.fillrect(0, 11, 2, 2, BALLCOLOR);

            while (1);  //終わり
          }
        }
      }
    }

  }

  //ゲームオーバー
  vga.clear(0);
  for (i = 0; i < 20; i++) {
    vga.fillrect(i, i, 1, 1, BALLCOLOR);
  }
  for (i = 20; i > 0; i--) {
    vga.fillrect(20 - i, i, 1, 1, BALLCOLOR);
  }
  while (1);  //終わり
}
