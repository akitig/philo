#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 共有データとミューテックスをまとめた構造体
typedef struct
{
	int counter;          // 共有のカウンタ
	pthread_mutex_t lock; // 排他制御用のミューテックス
}		SharedData;

// joinable（待機可能な）スレッド用の関数
void	*joinableThreadFunc(void *arg)
{
	SharedData	*data;

	data = (SharedData *)arg;
	for (int i = 0; i < 3; i++)
	{
		// ミューテックスをロックして共有データにアクセス
		pthread_mutex_lock(&data->lock);
		data->counter += 1; // 1 を加算
		printf("[Joinable] 更新後 counter = %d\n", data->counter);
		pthread_mutex_unlock(&data->lock);
		sleep(1); // 1秒待機
	}
	return (NULL);
}

// detached（待たずに終了する）スレッド用の関数
void	*detachedThreadFunc(void *arg)
{
	SharedData	*data;

	data = (SharedData *)arg;
	for (int i = 0; i < 3; i++)
	{
		pthread_mutex_lock(&data->lock);
		data->counter += 10; // 10 を加算
		printf("[Detached] 更新後 counter = %d\n", data->counter);
		pthread_mutex_unlock(&data->lock);
		sleep(1);
	}
	return (NULL);
}

int	main(void)
{
	SharedData data;
	data.counter = 0;
	// ミューテックスの初期化
	pthread_mutex_init(&data.lock, NULL);

	pthread_t t1,
		t2 =

			// joinable スレッドを作成（終了を待つ）
		pthread_create(&t1, NULL, joinableThreadFunc, &data);
	// detached スレッドを作成（終了を待たず自動解放）
	pthread_create(&t2, NULL, detachedThreadFunc, &data);
	pthread_detach(t2); // t2 をデタッチ状態に設定

	// joinable スレッドの終了を待つ
	pthread_join(t1, NULL);
	printf("hello\n");
	// 最終結果の出力
	printf("最終結果: counter = %d\n", data.counter);

	// ミューテックスの破棄
	pthread_mutex_destroy(&data.lock);

	return (0);
}