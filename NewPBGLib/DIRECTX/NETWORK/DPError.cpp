/*
 *   DPError.cpp   : DPlay エラー
 *
 */

#include "DPError.h"
#include "PbgError.h"



#ifdef PBG_DEBUG



FVOID DPlayError(HRESULT hr)
{
	char *pErr = "";

	switch(hr){
	case DP_OK:
		pErr = "要求は正しく完了した。 ";
	break;

	case DPERR_ABORTED:
		pErr = "処理は、完了する前にキャンセルされた。 ";
	break;

	case DPERR_ACCESSDENIED:
		pErr = "セッションが定員に達しているか、または不正なパスワードが指定された。 ";
	break;

	case DPERR_ACTIVEPLAYERS:
		pErr = "アクティブなプレーヤーが既に存在するため、要求された処理を実行できない。";
	break;

	case DPERR_ALREADYINITIALIZED:
		pErr = "オブジェクトは既に初期化されている。 ";
	break;

	case DPERR_APPNOTSTARTED:
		pErr = "アプリケーションはまだ開始されていない。 ";
	break;

	case DPERR_AUTHENTICATIONFAILED:
		pErr = "パスワードまたは身元証明を認証できなかった。 ";
	break;

	case DPERR_BUFFERTOOLARGE:
		pErr = "データ バッファが大きすぎて格納できない。 ";
	break;

	case DPERR_BUFFERTOOSMALL:
		pErr = "指定されたバッファが小さすぎて、要求されたデータを格納できない。 ";
	break;

	case DPERR_BUSY:
		pErr = "転送メディアがビジーであるため、メッセージを送信できない。 ";
	break;

	case DPERR_CANCELFAILED:
		pErr = "メッセージをキャンセルできなかった。おそらくこのメッセージは、"
		"グループ内の 1 人以上のメンバに既に送信されたグループ メッセージである。 ";
	break;

	case DPERR_CANCELLED:
		pErr = "処理がキャンセルされた。 ";
	break;

	case DPERR_CANNOTCREATESERVER:
		pErr = "新しいセッションに対するサーバーを作成できない。 ";
	break;

	case DPERR_CANTADDPLAYER:
		pErr = "プレーヤーをセッションに追加できない。 ";
	break;

	case DPERR_CANTCREATEGROUP:
		pErr = "新しいグループを作成できない。 ";
	break;

	case DPERR_CANTCREATEPLAYER:
		pErr = "新しいプレーヤーを作成できない。 ";
	break;

	case DPERR_CANTCREATEPROCESS:
		pErr = "アプリケーションを開始できない。 ";
	break;

	case DPERR_CANTCREATESESSION:
		pErr = "新しいセッションを作成できない。 ";
	break;

	case DPERR_CANTLOADCAPI:
		pErr = "身元情報が提供されておらず、暗号化サービスに使用する"
		" CryptoAPI パッケージ (CAPI) をロードできない。 ";
	break;

	case DPERR_CANTLOADSECURITYPACKAGE:
		pErr = "ソフトウェア セキュリティ パッケージをロードできない。 ";
	break;

	case DPERR_CANTLOADSSPI:
		pErr = "身元情報が提供されておらず、身元情報を要求する"
		" Security Support Provider Interface (SSPI) をロードできない。 ";
	break;

	case DPERR_CAPSNOTAVAILABLEYET:
		pErr = "DirectPlay オブジェクトの能力が、まだ決められていない。"
		"このエラーは、DirectPlay オブジェクトが、ポーリングによって利用可能な"
		"帯域幅と遅延時間を判断する接続ソリューションに実装されている場合に発生する。 ";
	break;

	case DPERR_CONNECTING:
		pErr = "メソッドはネットワークへの接続処理中である。"
		"アプリケーションでは、正しく完了したことを示す DP_OK か、"
		"または異なるエラーが返されるまで、このメソッドを使い続ける必要がある。 ";
	break;

	case DPERR_CONNECTIONLOST:
		pErr = "データの送信中に、サービス プロバイダの接続がリセットされた。 ";
	break;

	case DPERR_ENCRYPTIONFAILED:
		pErr = "要求された情報を暗号化できなかった。暗号化は、"
		"メッセージのプライバシを保護するために利用される。"
		"このエラーは、保証セッションでのみ発生する。 ";
	break;

	case DPERR_EXCEPTION:
		pErr = "要求を処理しているときに例外が発生した。 ";
	break;

	case DPERR_GENERIC:
		pErr = "定義されていないエラー条件が発生した。 ";
	break;

	case DPERR_INVALIDFLAGS:
		pErr = "このメソッドに渡されたフラグは無効である。 ";
	break;

	case DPERR_INVALIDGROUP:
		pErr = "グループ ID が、このゲーム セッションに対する有効な"
		"グループ ID として認識されていない。 ";
	break;

	case DPERR_INVALIDINTERFACE:
		pErr = "インターフェイス パラメータが無効である。 ";
	break;

	case DPERR_INVALIDOBJECT:
		pErr = "DirectPlay オブジェクトが無効である。 ";
	break;

	case DPERR_INVALIDPARAMS:
		pErr = "メソッドに渡された 1 つ以上のパラメータが無効である。 ";
	break;

	case DPERR_INVALIDPASSWORD:
		pErr = "パスワードを要求するセッションに参加するときに、"
		"無効なパスワードが提供された。 ";
	break;

	case DPERR_INVALIDPLAYER:
		pErr = "プレーヤー ID が、このゲーム セッションに対する有効な"
		"プレーヤー ID として認識されていない。 ";
	break;

	case DPERR_INVALIDPRIORITY:
		pErr = "指定された優先順位が、指定可能な範囲内 (0 ～ 65535) ではない。 ";
	break;

	case DPERR_LOGONDENIED:
		pErr = "身元証明が要求されているか、身元証明が提示されなかったか、"
		"または身元証明が無効であったため、セッションを開くことができなかった。 ";
	break;

	case DPERR_NOCAPS:
		pErr = "DirectPlay が使用する通信リンクが、この関数に対応していない。 ";
	break;

	case DPERR_NOCONNECTION:
		pErr = "通信リンクが確立されていない。 ";
	break;

	case DPERR_NOINTERFACE:
		pErr = "インターフェイスがサポートされていない。 ";
	break;

	case DPERR_NOMESSAGES:
		pErr = "受信キューにメッセージが存在しない。 ";
	break;

	case DPERR_NONAMESERVERFOUND:
		pErr = "ネーム サーバー (ホスト) が見つからないか、または作成できなかった。"
		"プレーヤーの作成にはホストが必要である。 ";
	break;

	case DPERR_NONEWPLAYERS:
		pErr = "セッションが新しいプレーヤーを受け入れていない。 ";
	break;

	case DPERR_NOPLAYERS:
		pErr = "セッション内にアクティブなプレーヤーが存在しない。 ";
	break;

	case DPERR_NOSESSIONS:
		pErr = "呼び出されたメソッドの対象となるセッションが存在しない。 ";
	break;

	case DPERR_NOTLOBBIED:
		pErr = "アプリケーションが DirectPlayLobby3.RunApplication メソッドによって"
		"開始されていない場合、またはこの DirectPlayLobby オブジェクトに対して"
		" DirectPlayLobbyConnection インターフェイスが現在初期化されていない場合に、DirectPlayLobby3.Connect メソッドから返される。 ";
	break;

	case DPERR_NOTLOGGEDIN:
		pErr = "プレーヤーまたはクライアント アプリケーションがログオンしていないため、"
		"動作を実行できない。クライアント アプリケーションがログオンせずに"
		"保証メッセージを送信したときに、DirectPlay4.Send メソッドから返される。 ";
	break;

	case DPERR_OUTOFMEMORY:
		pErr = "要求された処理を実行するために必要な量のメモリが存在しない。 ";
	break;

	case DPERR_PENDING:
		pErr = "エラーではない。非同期送信が、正しくキューに入れられるポイントに"
		"達したことを示す。詳細については、「SendEx」を参照すること。 ";
	break;

	case DPERR_PLAYERLOST:
		pErr = "プレーヤーがセッションへの接続を失った。 ";
	break;

	case DPERR_SENDTOOBIG:
		pErr = "DirectPlay4.Send メソッドによって送信されたメッセージが大きすぎる。 ";
	break;

	case DPERR_SESSIONLOST:
		pErr = "セッションへの接続が失われた。 ";
	break;

	case DPERR_SIGNFAILED:
		pErr = "要求された情報にデジタル署名を追加できない。デジタル署名は、"
		"メッセージが本物であることを示すために使用される。 ";
	break;

	case DPERR_TIMEOUT:
		pErr = "指定された時間内に処理を完了できなかった。 ";
	break;

	case DPERR_UNAVAILABLE:
		pErr = "要求された関数は現在利用できない。 ";
	break;

	case DPERR_UNINITIALIZED:
		pErr = "要求されたオブジェクトが初期化されていない。 ";
	break;

	case DPERR_UNKNOWNAPPLICATION:
		pErr = "不明なアプリケーションが指定された。 ";
	break;

	case DPERR_UNKNOWNMESSAGE:
		pErr = "メッセージ ID が有効ではない。キャンセルするメッセージの ID が"
		"無効な場合に、DirectPlay4.CancelMessage から返される。 ";
	break;

	case DPERR_UNSUPPORTED:
		pErr = "関数または機能が、この実装またはサービス プロバイダでは利用できない。"
		"ロビー セッションではないセッションから DirectPlay4.SetGroupConnectionSettings"
		" メソッドが呼び出された場合に、返される。また、優先順位またはタイムアウトが"
		"設定されていて、これらの機能がサービス プロバイダによってサポートされておらず、"
		"DirectPlay プロトコルがオンになっていない場合に、DirectPlay4.SendEx から返される。"
		"さらに、送信キューをチェックして、それがサービス プロバイダによってサポート"
		"されておらず、DirectPlay プロトコルがオンになっていない場合は、"
		"DirectPlay4.GetMessageQueue から返される。 ";
	break;

	case DPERR_USERCANCEL:
		pErr = "ユーザーが DirectPlay4.Open メソッドの呼び出し中に接続処理を"
		"キャンセルした場合、またはユーザーが DirectPlay4.GetDPEnumSessions の"
		"呼び出し中に DirectPlay サービス プロバイダのダイアログ ボックスで"
		" [キャンセル] をクリックした場合に返される。 ";
	break;

	default:
		pErr = "しらんぞぉ";
	break;
	}

	PbgError(pErr);
}



#endif
