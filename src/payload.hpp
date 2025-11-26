#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

// Arduino String の前方宣言
class String;

/**
 * 距離データ用の JSON ペイロードを生成
 * @param distance 距離値（cm）
 * @param fieldId フィールド識別子（例："Yokosuka-A1"）
 * @param ts オプションのタイムスタンプ（ミリ秒）（0 で省略）
 * @return JSON ペイロード文字列
 */
String buildPayload(float distance, const char* fieldId, unsigned long ts = 0);

#endif // PAYLOAD_HPP
