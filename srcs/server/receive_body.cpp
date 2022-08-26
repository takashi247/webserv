#include "receive_body.hpp"

#include <iostream>

#include "wrapper.hpp"

static const int kCRLFSize = 2;

static size_t SizeMin(size_t a, size_t b) {
  if (a < b) return a;
  return b;
}

/*
 * -1 : 読み込みエラー
 * other : Chunkサイズ
 */
static int GetChunkSize(std::string::const_iterator *it) {
  std::string::const_iterator cur = *it;
  int size = 0;
  while (('0' <= *cur && *cur <= '9') || ('A' <= *cur && *cur <= 'F') ||
         ('a' <= *cur && *cur <= 'f')) {
    size *= 16;
    if ('0' <= *cur && *cur <= '9')
      size += *cur - '0';
    else if ('A' <= *cur && *cur <= 'F')
      size += *cur - 'A' + 10;
    else if ('a' <= *cur && *cur <= 'f')
      size += *cur - 'a' + 10;
    ++cur;
  }
  if (*cur != '\r' || *(cur + 1) != '\n') {
    Wrapper::PrintMsg("Unexpected Separator");
    return -1;
  }
  *it = cur + kCRLFSize;
  return size;
}

/*
 * -1 : エラー切断
 * 0 : 読み込み完了
 * 1 : まだ読み込む
 */
int ParseNormalBody(size_t &remain_size, std::string &in, size_t &pos,
                    std::string &out) {
  size_t read_size = in.size() - pos;
  if (remain_size < read_size) {
    Wrapper::PrintMsg("Read Error over Content-Length");
    return -1;
  }
  out.append(in, pos, read_size);
  pos += read_size;
  remain_size -= read_size;
  if (remain_size == 0) return 0;
  return 1;
}

/*
 * すでに読み込んだメッセージからChunkedされた部分をパースする
 * -1: エラー切断
 * 0 : parse完了
 * 1 : まだ読み込む
 */
int ParseChunkedBody(size_t &remain_size, std::string &in, size_t &pos,
                     std::string &out) {
  // pos から続きを読み始める。
  if (remain_size == 0) {
    if (std::string::npos == in.find("\r\n", pos)) {
      return 1;  // サイズが無いのに、終端が見つからなければ、次を読む
    }
    std::string::const_iterator it = in.begin() + pos;
    int chunk_size = GetChunkSize(&it);
    // chunked bodyの先頭を指す。
    pos = in.find("\r\n", pos) + kCRLFSize;
    if (chunk_size == -1) {  //これでも読めないならエラー
      return 1;
    } else if (chunk_size == 0) {
      return 0;  //正常終了
    }
    // chunk_sizeと終端の\r\nを含んだ数だけ読み込む
    remain_size = chunk_size + kCRLFSize;
  }
  // posの位置から、remain_size分を読み込みたい
  // 読み込みたいchunk_sizeと、読み込み済み残りサイズの小さい方をbodyに追加する
  size_t append_size = SizeMin(remain_size, in.size() - pos);
  out.append(in, pos, append_size);
  // chunk最後の\r\nも追加されてるため、削除する
  size_t rfind_res = out.rfind("\r\n");
  if (rfind_res != std::string::npos && rfind_res == out.size() - 2) {
    out.erase(rfind_res);
  }
  pos += append_size;
  remain_size -= append_size;
  //まだ読み込んだ分が残っていて、次のchunkをパースするため、再帰呼び出し
  if (remain_size == 0) {
    return ParseChunkedBody(remain_size, in, pos, out);
  }
  return 1;
}
