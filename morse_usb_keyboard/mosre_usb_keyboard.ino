/*
* morse_usb_keyboard
* 2021.Aug.02, 7M4MON
* Decode morse code and send by USB keyboard with Arduino micro.
* License : GPLv3
*/

#include "Keyboard.h"

#define PIN_KEYER_IN        9
#define PIN_CHECK_MODE      14
#define PIN_BUZZER_OUT      10
#define PIN_TONE_FREQ_VOL   A3
#define PIN_SPEED_VOL       A0

#define BOUNCING_MS 20          // チャタリング無視時間
#define LETTER_DIV_DOTS 2       // TYP:3
#define WORD_DIV_DOTS 5         // TYP:7
#define MINIMUM_DOT_MS 20
#define COUNT_STOP_MS 10000     // 65.5秒以上押されるとオーバーフローする対策

/* dot = 0b01, dash = 0b11 */
#define MORSE_CODE_A 0b0111
#define MORSE_CODE_B 0b11010101
#define MORSE_CODE_C 0b11011101
#define MORSE_CODE_D 0b110101
#define MORSE_CODE_E 0b01
#define MORSE_CODE_F 0b01011101
#define MORSE_CODE_G 0b111101
#define MORSE_CODE_H 0b01010101
#define MORSE_CODE_I 0b0101
#define MORSE_CODE_J 0b01111111
#define MORSE_CODE_K 0b110111
#define MORSE_CODE_L 0b01110101
#define MORSE_CODE_M 0b1111
#define MORSE_CODE_N 0b1101
#define MORSE_CODE_O 0b111111
#define MORSE_CODE_P 0b01111101
#define MORSE_CODE_Q 0b11110111
#define MORSE_CODE_R 0b011101
#define MORSE_CODE_S 0b010101
#define MORSE_CODE_T 0b11
#define MORSE_CODE_U 0b010111
#define MORSE_CODE_V 0b01010111
#define MORSE_CODE_W 0b011111
#define MORSE_CODE_X 0b11010111
#define MORSE_CODE_Y 0b11011111
#define MORSE_CODE_Z 0b11110101
#define MORSE_CODE_0 0b1111111111
#define MORSE_CODE_1 0b0111111111
#define MORSE_CODE_2 0b0101111111
#define MORSE_CODE_3 0b0101011111
#define MORSE_CODE_4 0b0101010111
#define MORSE_CODE_5 0b0101010101
#define MORSE_CODE_6 0b1101010101
#define MORSE_CODE_7 0b1111010101
#define MORSE_CODE_8 0b1111110101
#define MORSE_CODE_9 0b1111111101

#define MORSE_CODE_QUESTION    0b010111110101   // ?
#define MORSE_CODE_EXCLAMATION 0b110111011111   // !
#define MORSE_CODE_SLASH  0b1101011101          // /
#define MORSE_CODE_PERIOD 0b011101110111        // .
#define MORSE_CODE_MINUS  0b110101010111        // -
#define MORSE_CODE_COMMA  0b111101011111        // ,
#define MORSE_CODE_PLUS   0b0111011101          // +
#define MORSE_CODE_AND    0b0111010101          // &
#define MORSE_CODE_EQUAL  0b1101010111          // =
#define MORSE_CODE_DBL_QUOTE 0b011101011101     // "
#define MORSE_CODE_BACKSPACE 0b0101010101010101 // BS
#define MORSE_CODE_LEFT_PAREN  0b1101111101     // (
#define MORSE_CODE_RIGHT_PAREN 0b110111110111   // )

#define MORSE_CODE__AR MORSE_CODE_PLUS          // +
#define MORSE_CODE__AS MORSE_CODE_AND           // &
#define MORSE_CODE__BT MORSE_CODE_EQUAL         // =
#define MORSE_CODE__KN MORSE_CODE_LEFT_PAREN    // (
#define MORSE_CODE__KK MORSE_CODE_RIGHT_PAREN   // )
#define MORSE_CODE__RR MORSE_CODE_DBLQUOTE      // "
#define MORSE_CODE__HH MORSE_CODE_BACKSPACE     // BS

#define ASC_CODE_BACKSPACE 0x08


uint16_t get_letter(uint16_t code){
    char c = 0;
    switch(code){
        case MORSE_CODE_A : c = 'a'; break;
        case MORSE_CODE_B : c = 'b'; break;
        case MORSE_CODE_C : c = 'c'; break;
        case MORSE_CODE_D : c = 'd'; break;
        case MORSE_CODE_E : c = 'e'; break;
        case MORSE_CODE_F : c = 'f'; break;
        case MORSE_CODE_G : c = 'g'; break;
        case MORSE_CODE_H : c = 'h'; break;
        case MORSE_CODE_I : c = 'i'; break;
        case MORSE_CODE_J : c = 'j'; break;
        case MORSE_CODE_K : c = 'k'; break;
        case MORSE_CODE_L : c = 'l'; break;
        case MORSE_CODE_M : c = 'm'; break;
        case MORSE_CODE_N : c = 'n'; break;
        case MORSE_CODE_O : c = 'o'; break;
        case MORSE_CODE_P : c = 'p'; break;
        case MORSE_CODE_Q : c = 'q'; break;
        case MORSE_CODE_R : c = 'r'; break;
        case MORSE_CODE_S : c = 's'; break;
        case MORSE_CODE_T : c = 't'; break;
        case MORSE_CODE_U : c = 'u'; break;
        case MORSE_CODE_V : c = 'v'; break;
        case MORSE_CODE_W : c = 'w'; break;
        case MORSE_CODE_X : c = 'x'; break;
        case MORSE_CODE_Y : c = 'y'; break;
        case MORSE_CODE_Z : c = 'z'; break;
        
        case MORSE_CODE_0 : c = '0'; break;
        case MORSE_CODE_1 : c = '1'; break;
        case MORSE_CODE_2 : c = '2'; break;
        case MORSE_CODE_3 : c = '3'; break;
        case MORSE_CODE_4 : c = '4'; break;
        case MORSE_CODE_5 : c = '5'; break;
        case MORSE_CODE_6 : c = '6'; break;
        case MORSE_CODE_7 : c = '7'; break;
        case MORSE_CODE_8 : c = '8'; break;
        case MORSE_CODE_9 : c = '9'; break;

        case MORSE_CODE_QUESTION    : c = '?'; break;
        case MORSE_CODE_EXCLAMATION : c = '!'; break;
        case MORSE_CODE_SLASH       : c = '/'; break;
        case MORSE_CODE_PERIOD      : c = '.'; break;
        case MORSE_CODE_MINUS       : c = '-'; break;
        case MORSE_CODE_COMMA       : c = ','; break;
        case MORSE_CODE_PLUS        : c = '+'; break;
        case MORSE_CODE_AND         : c = '&'; break;
        case MORSE_CODE_EQUAL       : c = '='; break;
        case MORSE_CODE_DBL_QUOTE   : c = '"'; break;
        case MORSE_CODE_LEFT_PAREN  : c = '('; break;
        case MORSE_CODE_RIGHT_PAREN : c = ')'; break;
        case MORSE_CODE_BACKSPACE   : c = ASC_CODE_BACKSPACE; break;

        default : break;
    }
    return c;
}

bool inputting, next_space;
uint16_t morse_code;
uint16_t det_counter, dot_len;
uint16_t tone_freq;
uint16_t bouncing_time;

void buzz_morse(uint16_t code){
    noTone(PIN_BUZZER_OUT);    // 念のため
    if(code != 0){
        while( (code & 0xC000) == 0 ) { code <<= 2;}   // 頭出し
        while( (code & 0xC000) != 0 ){
            uint16_t t = (code & 0xC000) >> 14;     // t = 1 or 3;
            tone(PIN_BUZZER_OUT, tone_freq);
            delay(dot_len * t);
            noTone(PIN_BUZZER_OUT);
            delay(dot_len);
            code <<= 2;
        }
        delay(dot_len * 3);
    }
}


void read_volumes(){
    dot_len = (analogRead(PIN_SPEED_VOL) >> 3) + MINIMUM_DOT_MS;    // 20 + 0~127 (240で旧3級、133で旧2級、100で旧1級）
    tone_freq = 500 + (analogRead(PIN_TONE_FREQ_VOL) >> 1);         // 最小500Hz, 最大1011Hz
}

void setup() {
    pinMode(PIN_KEYER_IN, INPUT_PULLUP);
    pinMode(PIN_CHECK_MODE, INPUT_PULLUP);
    inputting = false;
    next_space = false;
    det_counter = 0;
    bouncing_time = BOUNCING_MS;
    morse_code = 0;
    read_volumes();     // dot_len, tone_freq の初期化
    Serial.begin(115200);
    Keyboard.begin();
    buzz_morse(MORSE_CODE_V);
}

void loop() {
    while (digitalRead(PIN_KEYER_IN) == HIGH){     // 押下待ち
        if(inputting){
            delay(1);
            det_counter++;                                  // 離された時間をカウント
            if(det_counter + bouncing_time > dot_len * LETTER_DIV_DOTS){    // 規定時間、離されていたら
                uint16_t letter = get_letter(morse_code);                   // 短長点の組み合わせから文字を決定
                if (letter != 0){
                    Keyboard.write(letter);                                 // キーボード送信
                    if (letter != ASC_CODE_BACKSPACE) next_space = true;    // 単語間のスペースのフラグを立てる
                }
                inputting = false;
                det_counter = 0;
                morse_code= 0;
            }
        }
        if(next_space){
            delay(1);
            det_counter++;
            if(det_counter > dot_len * WORD_DIV_DOTS){
                Keyboard.write(' ');                    // 単語間のスペースを送出
                next_space = false;
                det_counter = 0;
            }
        }
        if(digitalRead(PIN_CHECK_MODE) == LOW){
            buzz_morse(MORSE_CODE_V);
        }
        read_volumes();
    }
    inputting = true;                           // キーが接になった。
    tone(PIN_BUZZER_OUT, tone_freq);                // トーン開始
    det_counter = 0;
    //delay(bouncing_time);                      // チャタリングの防止（旧）
    uint16_t bounce_counter = 0;
    while (bounce_counter < bouncing_time){     // 離されるのを待つ
        if(digitalRead(PIN_KEYER_IN) == LOW) {
            bounce_counter = 0;                 // チャタリングを考慮して、連続してbouncing_time回 Hになったら抜ける方式に変更。
        }else{
            bounce_counter++;
        }
        delay(1);
        det_counter++;                      // 押されている時間をカウント
        det_counter = min(det_counter, COUNT_STOP_MS);
    }
    // 離された
    noTone(PIN_BUZZER_OUT);    // トーン停止
    morse_code <<= 2;      // 2ビットシフト
    uint16_t dot_dash = (det_counter - bouncing_time > dot_len * 2) ? 3 : 1;    // 離された時、カウントが閾値以下だったら 1, 閾値以上だったら 3
    morse_code |= dot_dash;
    det_counter = 0;
    //delay(bouncing_time);                      // チャタリングの防止（旧）
}
