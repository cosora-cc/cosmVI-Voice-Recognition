#ifndef COSM_VI_H
#define COSM_VI_H

#include <stdint.h>
#include <stddef.h>

#if defined(_WIN32)
#ifdef COSMVI_EXPORTS
#define COSMVI_LIB_API __declspec(dllexport)
#else
#define COSMVI_LIB_API __declspec(dllimport)
#endif
#else
// Linux (GCC/Clang) の設定
#if __GNUC__ >= 4
#define COSMVI_LIB_API __attribute__ ((visibility ("default")))
#else
#define COSMVI_LIB_API
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

    //==============================================================================
    // ハンドルと基本型
    //==============================================================================

    /**
     * @brief ライブラリのインスタンスを指す不透明ポインタ。
     *        全てのAPI関数はこのハンドルを介して操作します。
     */
    typedef struct cosmVI_Context* cosmVI_Handle;

    /**
     * @brief API関数の結果を示すステータスコード。
     */
    typedef enum cosmVI_Status {
        COSMVI_SUCCESS = 0,
        COSMVI_INFO_QUEUE_EMPTY = 1, // PollEvent専用: キューは空だった
        COSMVI_ERROR_INVALID_HANDLE,
        COSMVI_ERROR_INVALID_ARGUMENT,
        COSMVI_ERROR_INITIALIZATION_FAILED,
        COSMVI_ERROR_ALREADY_INITIALIZED,
        COSMVI_ERROR_NOT_INITIALIZED,
        COSMVI_ERROR_ALREADY_RUNNING,
        COSMVI_ERROR_NOT_RUNNING,
        COSMVI_ERROR_INVALID_STATE, // 不正な状態でのAPI呼び出し
        COSMVI_ERROR_AUDIO_ENGINE_FAILED,
        // ... その他の詳細なエラーコード
        COSMVI_ERROR_UNKNOWN_ERROR
    } cosmVI_Status;

    /**
     * @brief ライブラリの内部状態
     */
    typedef enum cosmVI_State {
        COSMVI_STATE_UNINITIALIZED,
        COSMVI_STATE_INITIALIZING,
        COSMVI_STATE_IDLE,       // 初期化完了、待機中
        COSMVI_STATE_RUNNING,    // 音声認識実行中
        COSMVI_STATE_PAUSED,     // 音声認識一時停止中
        COSMVI_STATE_STOPPING,
        COSMVI_STATE_ERROR
    } cosmVI_State;

    //==============================================================================
    // イベントシステム
    //==============================================================================

    /**
     * @brief イベントの種類。
     */
    typedef enum cosmVI_EventType {
        COSMVI_EVENT_NONE = 0,                 // キューにイベントがない
        COSMVI_EVENT_PARTIAL_RESULT,           // 中間認識結果
        COSMVI_EVENT_FINAL_RESULT,             // 最終認識結果
        COSMVI_EVENT_STATE_CHANGED,            // ライブラリの状態が変化した
        COSMVI_EVENT_ERROR                     // エラーが発生した
    } cosmVI_EventType;

    /**
     * @brief イベントの詳細情報を格納する構造体。
     */
#define COSMVI_MAX_TEXT_LENGTH 256
#define COSMVI_MAX_ERROR_MSG_LENGTH 256

    typedef struct cosmVI_Event {
        cosmVI_EventType type;
        union {
            // EVENT_PARTIAL_RESULT, EVENT_FINAL_RESULT
            char result_text[COSMVI_MAX_TEXT_LENGTH];

            // EVENT_STATE_CHANGED
            cosmVI_State new_state;

            // EVENT_ERROR
            struct {
                cosmVI_Status error_code;
                char error_message[COSMVI_MAX_ERROR_MSG_LENGTH];
            } error_info;
        } data;
    } cosmVI_Event;


    //==============================================================================
    // ライフサイクルと設定
    //==============================================================================

    /**
     * @brief cosmVIライブラリのインスタンスを作成します。
     * @param[out] out_handle 作成されたインスタンスへのハンドル。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_Create(cosmVI_Handle* out_handle);

    /**
     * @brief cosmVIライブラリのインスタンスを破棄し、関連する全てのリソースを解放します。
     * @param[in,out] handle 破棄するインスタンスのハンドル。関数呼び出し後、ハンドルは無効になります。
     */
    COSMVI_LIB_API void cosmVI_Destroy(cosmVI_Handle* handle);

    /**
     * @brief 音声認識エンジンを初期化します。
     * @param handle インスタンスハンドル。
     * @param model_path Voskモデルが格納されているディレクトリのパス。
     *  * @param sample_rate 音声認識に使用するサンプルレート(Hz)。
     *                       Voskが推奨する16000.0fが一般的です。
     *                       0以下の値を指定すると、ライブラリのデフォルト値(16000.0f)が使用されます。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_Initialize(cosmVI_Handle handle, const char* model_path, float sample_rate);

    /**
     * @brief 音声認識に使用するスレッド数を動的に設定します。
     *        認識実行中でも安全に呼び出すことが可能です。
     * @param handle インスタンスハンドル。
     * @param num_threads 割り当てるスレッド数。0を指定するとハードウェアの同時実行スレッド数に設定されます。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_SetThreadCount(cosmVI_Handle handle, unsigned int num_threads);

    /**
     * @brief 認識対象の語彙リスト（文法）を動的に設定します。
     *        認識実行中に呼び出すと、次の認識から新しい文法が適用されます。
     * @param handle インスタンスハンドル。
     * @param grammar_json UTF-8エンコードされたJSON形式の文法文字列。例: "[\"hello world\", \"command open\", \"[unk]\"]"
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_SetGrammar(cosmVI_Handle handle, const char* grammar_json);


    //==============================================================================
    // 音声認識処理
    //==============================================================================

    /**
     * @brief 音声認識セッションを開始します。
     *        内部でオーディオ入力の受付とバックグラウンド処理を開始します (FR-03)。
     * @param handle インスタンスハンドル。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_Start(cosmVI_Handle handle);

    /**
     * @brief 実行中の音声認識を一時停止します。
     *        オーディオデータの処理を中断しますが、セッションは維持されます。
     *        この関数は、状態が RUNNING の場合にのみ有効です。
     * @param handle インスタンスハンドル。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_Pause(cosmVI_Handle handle);

    /**
     * @brief 一時停止中の音声認識を再開します。
     *        この関数は、状態が PAUSED の場合にのみ有効です。
     * @param handle インスタンスハンドル。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_Resume(cosmVI_Handle handle);


    /**
     * @brief 音声認識セッションを停止します。
     * @param handle インスタンスハンドル。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_Stop(cosmVI_Handle handle);

    /**
     * @brief 外部から音声データをライブラリに供給します。
     * @param handle インスタンスハンドル。
     * @param audio_buffer 16-bit PCMモノラル形式の音声データ。
     * @param buffer_size バッファのサイズ（バイト単位）。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_FeedAudio(cosmVI_Handle handle, const int16_t* audio_buffer, size_t buffer_size);


    //==============================================================================
    // 結果と状態の取得
    //==============================================================================

    /**
     * @brief イベントキューからイベントを1つ取り出します。キューが空の場合はすぐに戻ります。
     * @param handle インスタンスハンドル。
     * @param[out] out_event 取り出されたイベントが格納される構造体へのポインタ。
     * @return イベントが存在した場合は COSMVI_SUCCESS。キューが空の場合は COSMVI_EVENT_NONE。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_PollEvent(cosmVI_Handle handle, cosmVI_Event* out_event);

    /**
     * @brief ライブラリの現在の状態を取得します。
     * @param handle インスタンスハンドル。
     * @param[out] out_state 現在の状態が格納される変数へのポインタ。
     * @return 成功した場合は COSMVI_SUCCESS。
     */
    COSMVI_LIB_API cosmVI_Status cosmVI_GetState(cosmVI_Handle handle, cosmVI_State* out_state);


#ifdef __cplusplus
}
#endif

#endif // COSM_VI_H