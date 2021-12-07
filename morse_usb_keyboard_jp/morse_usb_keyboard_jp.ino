/*
* morse_usb_keyboard_jp
* 2021.Aug.08, 7M4MON
* Decode morse code and send by USB keyboard with Arduino micro. の和文対応バージョン
* License : GPLv3
*/
/* 
* 変更履歴
* 2021-12-06(1)　パドル入力に対応した。
* 2021-12-06(2)  設定値をeepromに保存するようにした。
* 2021-12-07     IMEの使用状態を選べるようにした。
*/
#define JAPANESE_KEYBOARD
#ifdef JAPANESE_KEYBOARD
#include "Keyboard_jp.h"    // 日本語キーボード
                            // https://mgt.blog.ss-blog.jp/2016-01-14
#else
#include "Keyboard.h"       // English Keyboard.
#endif

#include <EEPROM.h>
#define EEP_ADDR_USE_PADDLE     0x00
#define EEP_ADDR_PADDLE_REVERSE 0x01
#define EEP_ADDR_MODE_KANA      0x02

#define PIN_KEYER_IN_L      8
#define PIN_KEYER_IN_R      9
#define PIN_CHECK_MODE      14
#define PIN_BUZZER_OUT      10
#define PIN_TONE_FREQ_VOL   A3
#define PIN_SPEED_VOL       A0

#define BOUNCING_MS 20          // チャタリング無視時間
#define LETTER_DIV_DOTS 2       // TYP:3
#define WORD_DIV_DOTS 6         // TYP:7
#define MINIMUM_DOT_MS 20       // 最速のキースピード (1dot:20ms = 60 WPM)
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

#define MORSE_CODE_W_A	0b1111011111
#define MORSE_CODE_W_I	0b0111
#define MORSE_CODE_W_U	0b010111
#define MORSE_CODE_W_E	0b1101111111
#define MORSE_CODE_W_O	0b0111010101
#define MORSE_CODE_W_KA	0b01110101
#define MORSE_CODE_W_KI	0b1101110101
#define MORSE_CODE_W_KU	0b01010111
#define MORSE_CODE_W_KE	0b11011111
#define MORSE_CODE_W_KO	0b11111111
#define MORSE_CODE_W_SA	0b1101110111
#define MORSE_CODE_W_SI	0b1111011101
#define MORSE_CODE_W_SU	0b1111110111
#define MORSE_CODE_W_SE	0b0111111101
#define MORSE_CODE_W_SO	0b11111101
#define MORSE_CODE_W_TA	0b1101
#define MORSE_CODE_W_TI	0b01011101
#define MORSE_CODE_W_TU	0b01111101
#define MORSE_CODE_W_TE	0b0111011111
#define MORSE_CODE_W_TO	0b0101110101
#define MORSE_CODE_W_NA	0b011101
#define MORSE_CODE_W_NI	0b11011101
#define MORSE_CODE_W_NU	0b01010101
#define MORSE_CODE_W_NE	0b11110111
#define MORSE_CODE_W_NO	0b01011111
#define MORSE_CODE_W_HA	0b11010101
#define MORSE_CODE_W_HI	0b1111010111
#define MORSE_CODE_W_HU	0b11110101
#define MORSE_CODE_W_HE	0b01
#define MORSE_CODE_W_HO	0b110101
#define MORSE_CODE_W_MA	0b11010111
#define MORSE_CODE_W_MI	0b0101110111
#define MORSE_CODE_W_MU	0b11
#define MORSE_CODE_W_ME	0b1101010111
#define MORSE_CODE_W_MO	0b1101011101
#define MORSE_CODE_W_YA	0b011111
#define MORSE_CODE_W_YI	0b0111010111
#define MORSE_CODE_W_YU	0b1101011111
#define MORSE_CODE_W_YE	0b0111110101
#define MORSE_CODE_W_YO	0b1111
#define MORSE_CODE_W_RA	0b010101
#define MORSE_CODE_W_RI	0b111101
#define MORSE_CODE_W_RU	0b1101111101
#define MORSE_CODE_W_RE	0b111111
#define MORSE_CODE_W_RO	0b01110111
#define MORSE_CODE_W_WA	0b110111
#define MORSE_CODE_W_WO	0b01111111
#define MORSE_CODE_W_NN	0b0111011101
	
#define MORSE_CODE_W_DAKU	 0b0101
#define MORSE_CODE_W_HANDAKU 0b0101111101
#define MORSE_CODE_W_CHOUON	 0b0111110111
	
#define MORSE_CODE_W_HORE	0b110101111111
#define MORSE_CODE_W_RATA	0b0101011101
#define MORSE_CODE_W_LKAKKO	0b011101011101
#define MORSE_CODE_W_RKAKKO	MORSE_CODE_RIGHT_PAREN

#define MORSE_CODE_MODE_PADDLE   0b01111101110101     // PD でパドルモード
#define MORSE_CODE_MODE_STRAIGHT 0b01010111011101     // STR でストレートキーモード
#define MORSE_CODE_MODE_REVERSE  0b0111010101010111   // REV でパドル接点反転モード
#define MORSE_CODE_MODE_NORMAL   0b1101111111011101   // NOR でパドル接点通常モード
#define MORSE_CODE_MODE_ALPHABET 0b011101111101       // ENG で英文モード
#define MORSE_CODE_MODE_KANA     0b11011101111101     // KAN で日本語モード

bool mode_wabun, tmp_eng;
bool kana_on, using_paddle, paddle_reverse;

/* 子音(consonant)をUSBキーボードに出力する */
/* かな入力時はIME ONにしてローマ字入力とすること */
char output_consonant(uint16_t code){
    char c = 0;
    switch(code){
        case MORSE_CODE_W_KA : ;
        case MORSE_CODE_W_KI : ;
        case MORSE_CODE_W_KU : ;
        case MORSE_CODE_W_KE : ;
        case MORSE_CODE_W_KO : c = 'k'; break;
        case MORSE_CODE_W_SA : ;
        case MORSE_CODE_W_SI : ;
        case MORSE_CODE_W_SU : ; 
        case MORSE_CODE_W_SE : ; 
        case MORSE_CODE_W_SO : c = 's'; break;
        case MORSE_CODE_W_TA : ;
        case MORSE_CODE_W_TI : ;
        case MORSE_CODE_W_TU : ;
        case MORSE_CODE_W_TE : ;
        case MORSE_CODE_W_TO : c = 't'; break;
        case MORSE_CODE_W_NA : ;
        case MORSE_CODE_W_NI : ;
        case MORSE_CODE_W_NU : ;
        case MORSE_CODE_W_NE : ;
        case MORSE_CODE_W_NO : ;
        case MORSE_CODE_W_NN : c = 'n'; break;
        case MORSE_CODE_W_HA : ;
        case MORSE_CODE_W_HI : ;
        case MORSE_CODE_W_HU : ;
        case MORSE_CODE_W_HE : ;
        case MORSE_CODE_W_HO : c = 'h'; break;
        case MORSE_CODE_W_MA : ;
        case MORSE_CODE_W_MI : ;
        case MORSE_CODE_W_MU : ;
        case MORSE_CODE_W_ME : ;
        case MORSE_CODE_W_MO : c = 'm'; break;
        case MORSE_CODE_W_YA : ;
        case MORSE_CODE_W_YI : ;
        case MORSE_CODE_W_YU : ;
        case MORSE_CODE_W_YE : ;
        case MORSE_CODE_W_YO : c = 'y'; break;
        case MORSE_CODE_W_RA : ;
        case MORSE_CODE_W_RI : ;
        case MORSE_CODE_W_RU : ;
        case MORSE_CODE_W_RE : ;
        case MORSE_CODE_W_RO : c = 'r'; break;
        case MORSE_CODE_W_WA : ;
        case MORSE_CODE_W_WO : c = 'w'; break;
    }
    if (c != 0) {
        Keyboard.write(c);
    }
    return c;
}

/* 母音(vowel),数値をUSBキーボードに出力する */
char output_vowelnumeric(uint16_t code){
    char c = 0;
    switch(code){
        case MORSE_CODE_W_A : ;
        case MORSE_CODE_W_KA : ;
        case MORSE_CODE_W_SA : ;
        case MORSE_CODE_W_TA : ;
        case MORSE_CODE_W_NA : ;
        case MORSE_CODE_W_HA : ;
        case MORSE_CODE_W_MA : ;
        case MORSE_CODE_W_YA : ;
        case MORSE_CODE_W_RA : ;
        case MORSE_CODE_W_WA : c = 'a'; break;
        case MORSE_CODE_W_I : ;
        case MORSE_CODE_W_KI : ;
        case MORSE_CODE_W_SI : ;
        case MORSE_CODE_W_TI : ;
        case MORSE_CODE_W_NI : ;
        case MORSE_CODE_W_HI : ;
        case MORSE_CODE_W_MI : ;
        case MORSE_CODE_W_YI : ;
        case MORSE_CODE_W_RI : c = 'i'; break;
        case MORSE_CODE_W_U : ;
        case MORSE_CODE_W_KU : ;
        case MORSE_CODE_W_SU : ;
        case MORSE_CODE_W_TU : ;
        case MORSE_CODE_W_NU : ;
        case MORSE_CODE_W_HU : ;
        case MORSE_CODE_W_MU : ;
        case MORSE_CODE_W_YU : ;
        case MORSE_CODE_W_RU : c = 'u'; break;
        case MORSE_CODE_W_E : ;
        case MORSE_CODE_W_KE : ;
        case MORSE_CODE_W_SE : ;
        case MORSE_CODE_W_TE : ;
        case MORSE_CODE_W_NE : ;
        case MORSE_CODE_W_HE : ;
        case MORSE_CODE_W_ME : ;
        case MORSE_CODE_W_YE : ;
        case MORSE_CODE_W_RE : c = 'e'; break;
        case MORSE_CODE_W_O : ;
        case MORSE_CODE_W_KO : ;
        case MORSE_CODE_W_SO : ;
        case MORSE_CODE_W_TO : ;
        case MORSE_CODE_W_NO : ;
        case MORSE_CODE_W_HO : ;
        case MORSE_CODE_W_MO : ;
        case MORSE_CODE_W_YO : ;
        case MORSE_CODE_W_RO : ;
        case MORSE_CODE_W_WO : c = 'o'; break;
        case MORSE_CODE_W_NN : c = 'n'; break;
        case MORSE_CODE_W_DAKU      : c = '\"'; break;
        case MORSE_CODE_W_HANDAKU   : c = '\''; break;
        case MORSE_CODE_W_CHOUON    : c = '-'; break;
        
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
        case MORSE_CODE_PERIOD      : c = '.'; break;
        case MORSE_CODE_MINUS       : c = '-'; break;
        case MORSE_CODE_COMMA       : c = ','; break;
        case MORSE_CODE_DBL_QUOTE   : c = '\"'; break;
        case MORSE_CODE_BACKSPACE   : c = ASC_CODE_BACKSPACE; break;

        case MORSE_CODE_W_RATA      : c = ']'; mode_wabun = false;  break;
        case MORSE_CODE_W_RKAKKO    : c = '('; tmp_eng = true; break;
    }
    if (c != 0) {
        Keyboard.write(c);
        if (kana_on) Keyboard.write(KEY_RETURN);     // 母音を入力したら確定 (IME ONかつローマ字入力にすること)
    }
    return c;
}

/* アルファベット,数値をUSBキーボードに出力する */
char output_alphanumeric(uint16_t code){
    char c = 0;
    switch(code){
        case MORSE_CODE_A : c = 'A'; break;     // 日本語IMEを使用するときは、Shift- （つまり大文字）を入力する。
        case MORSE_CODE_B : c = 'B'; break;
        case MORSE_CODE_C : c = 'C'; break;
        case MORSE_CODE_D : c = 'D'; break;
        case MORSE_CODE_E : c = 'E'; break;
        case MORSE_CODE_F : c = 'F'; break;
        case MORSE_CODE_G : c = 'G'; break;
        case MORSE_CODE_H : c = 'H'; break;
        case MORSE_CODE_I : c = 'I'; break;
        case MORSE_CODE_J : c = 'J'; break;
        case MORSE_CODE_K : c = 'K'; break;
        case MORSE_CODE_L : c = 'L'; break;
        case MORSE_CODE_M : c = 'M'; break;
        case MORSE_CODE_N : c = 'N'; break;
        case MORSE_CODE_O : c = 'O'; break;
        case MORSE_CODE_P : c = 'P'; break;
        case MORSE_CODE_Q : c = 'Q'; break;
        case MORSE_CODE_R : c = 'R'; break;
        case MORSE_CODE_S : c = 'S'; break;
        case MORSE_CODE_T : c = 'T'; break;
        case MORSE_CODE_U : c = 'U'; break;
        case MORSE_CODE_V : c = 'V'; break;
        case MORSE_CODE_W : c = 'W'; break;
        case MORSE_CODE_X : c = 'X'; break;
        case MORSE_CODE_Y : c = 'Y'; break;
        case MORSE_CODE_Z : c = 'Z'; break;

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
        case MORSE_CODE_RIGHT_PAREN : c = ')'; if (mode_wabun) tmp_eng = false; break;
        case MORSE_CODE_BACKSPACE   : c = ASC_CODE_BACKSPACE; break;

        case MORSE_CODE_W_HORE      : c = '['; mode_wabun = true; break;    // 和文モードに入る
    }
    if (c != 0) {
        Keyboard.write(c);
        if (kana_on) Keyboard.write(KEY_RETURN);     // 日本語IMEにおいては、英数字は即確定する。
    }
    return c;
}

/* モード切替入力 */
char input_mode_change(uint16_t code){
    switch(code){
        case MORSE_CODE_MODE_PADDLE : 
            using_paddle = true;
            EEPROM.write(EEP_ADDR_USE_PADDLE,1);
            tone(PIN_BUZZER_OUT, 2000, 500);
            break;
        case MORSE_CODE_MODE_STRAIGHT : 
            using_paddle = false;
            EEPROM.write(EEP_ADDR_USE_PADDLE,0);
            tone(PIN_BUZZER_OUT, 2000, 500);
            break;
        case MORSE_CODE_MODE_REVERSE : 
            paddle_reverse = true;
            EEPROM.write(EEP_ADDR_PADDLE_REVERSE,1);
            tone(PIN_BUZZER_OUT, 2000, 500);
            break;
        case MORSE_CODE_MODE_NORMAL : 
            paddle_reverse = false;
            EEPROM.write(EEP_ADDR_PADDLE_REVERSE,0);
            tone(PIN_BUZZER_OUT, 2000, 500);
            break;
        case MORSE_CODE_MODE_KANA : 
            kana_on = true;
            EEPROM.write(EEP_ADDR_MODE_KANA,1);
            tone(PIN_BUZZER_OUT, 2000, 500);
            break;
        case MORSE_CODE_MODE_ALPHABET : 
            kana_on = false;
            EEPROM.write(EEP_ADDR_MODE_KANA,0);
            tone(PIN_BUZZER_OUT, 2000, 500);
            break;
        default: break;
    }
    return 0;
}
    

/* 入力した文字をUSBキーボードとしてPCに出力する */
char output_keybord(uint16_t code){
    char result = 0;
    input_mode_change(code);
    if(mode_wabun && !tmp_eng){
        result = output_consonant(code);
        result = output_vowelnumeric(code);  
    }else{
        result = output_alphanumeric(code);
    }
    return result;
}

bool inputting, next_space;
uint16_t morse_code;
uint16_t dot_len;
uint16_t tone_freq;
uint16_t bouncing_time;

/* 指定のモールスコードをブザーから鳴らす */
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

/* 現在の設定状態をモールスで鳴らす */
void buzz_state(){
    if (using_paddle) buzz_morse(MORSE_CODE_P);
    else buzz_morse(MORSE_CODE_S);

    if (paddle_reverse) buzz_morse(MORSE_CODE_R);
    else buzz_morse(MORSE_CODE_N);

    if (kana_on) buzz_morse(MORSE_CODE_K);
    else buzz_morse(MORSE_CODE_E);
}


/* 接点を読み取って、パドルの状態を返す */
/* 開放 = 0, 短点 = 1, 長点 = 2, 両方 = 3 */
uint8_t input_keyer(bool paddle_reverse){
    uint8_t paddle_state = 0;
    if (digitalRead(PIN_KEYER_IN_L) == LOW){
        paddle_state = paddle_reverse ? 2 : 1;
    }
    if (digitalRead(PIN_KEYER_IN_R) == LOW){
        paddle_state += paddle_reverse ? 1 : 2;
    }
    return paddle_state;
}

/* パドル使用時に状態に応じて長短点を再生する */
uint8_t play_dot_dash(uint8_t state){
    static uint8_t last_play = 0;
    uint8_t dot_dash = 0;
    tone(PIN_BUZZER_OUT, tone_freq);
    switch (state){
        case 1:     //dot
            dot_dash = 1;
            break;
        case 2:     //dash
            dot_dash = 3;
            break;
        case 3:     //両押しはスクイーズ動作
            dot_dash = (last_play == 1) ? 3 : 1;
            break;
        default:
            dot_dash = 0;
            break;
    }
    delay(dot_len * dot_dash);
    last_play = dot_dash;
    noTone(PIN_BUZZER_OUT);
    delay(dot_len);
    return last_play;
}

/* バグキーやストレートキー使用時に長短点を決定する */
uint8_t detect_dot_dash(){
    tone(PIN_BUZZER_OUT, tone_freq);                // トーン開始
    uint16_t det_counter = 0;
    uint16_t bounce_counter = 0;
    while (bounce_counter < bouncing_time){     // 離されるのを待つ
        if(input_keyer(paddle_reverse) != 0) {
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
    return (det_counter - bouncing_time > dot_len * 2) ? 3 : 1;    // 離された時、カウントが閾値以下だったら 1, 閾値以上だったら 3
}

/* トーン周波数と速度を半固定抵抗から読みとる */
void read_volumes(){
    dot_len = (analogRead(PIN_SPEED_VOL) >> 3) + MINIMUM_DOT_MS;    // 20 + 0~127 60～8 WPM (240で旧3級、133で旧2級、100で旧1級）
    tone_freq = 500 + (analogRead(PIN_TONE_FREQ_VOL) >> 1);         // 最小500Hz, 最大1011Hz
}

/* ボタンを押しながら電源ONでリセット */
void reset_settings(){
    EEPROM.write(EEP_ADDR_USE_PADDLE,0);
    EEPROM.write(EEP_ADDR_PADDLE_REVERSE,0);
    EEPROM.write(EEP_ADDR_MODE_KANA,0);
    tone(PIN_BUZZER_OUT, 2000, 1000);
    while(digitalRead(PIN_CHECK_MODE) == LOW){;}    //離されるのを待つ
    delay(200);
}

/* 初期化処理 */
void setup() {
    pinMode(PIN_KEYER_IN_L, INPUT_PULLUP);
    pinMode(PIN_KEYER_IN_R, INPUT_PULLUP);
    pinMode(PIN_CHECK_MODE, INPUT_PULLUP);
    inputting = false;
    next_space = false;
    bouncing_time = BOUNCING_MS;
    morse_code = 0;
    read_volumes();     // dot_len, tone_freq の初期化
    if (digitalRead(PIN_CHECK_MODE) == LOW) reset_settings();
    using_paddle = EEPROM.read(EEP_ADDR_USE_PADDLE);
    paddle_reverse = EEPROM.read(EEP_ADDR_PADDLE_REVERSE);
    kana_on = EEPROM.read(EEP_ADDR_MODE_KANA);
    mode_wabun = false;
    tmp_eng = false;
    Serial.begin(115200);
    Keyboard.begin();
    buzz_state();
}

/* メインループ */
void loop() {
    uint8_t keying_state = 0;
    uint16_t det_counter = using_paddle ? dot_len : bouncing_time;  // カウンタの初期値
    while (keying_state == 0){     // 押下待ち
        keying_state = input_keyer(paddle_reverse);
        if(inputting){
            delay(1);
            det_counter++;                                  // 離された時間をカウント
            if(det_counter > dot_len * LETTER_DIV_DOTS){    // 規定時間、離されていたら
                char letter = output_keybord(morse_code);                        // 文字を入力
                if ((letter != 0) && (letter != ASC_CODE_BACKSPACE)) next_space = true;    // 単語間のスペースのフラグを立てる
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
    uint16_t dot_dash;
    if(using_paddle){  /* パドルの場合 */
        dot_dash = (uint16_t)play_dot_dash(keying_state);
    }else{             /* バグキーやストレートキーの場合 */
        dot_dash = (uint16_t)detect_dot_dash();
    }
    morse_code <<= 2;       // 左2ビットシフト
    morse_code |= dot_dash; // 最下位2bitに 01 or 11 を入れる
}
