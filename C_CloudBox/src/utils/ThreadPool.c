#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

// 定义一个任务结构体
typedef struct task {
    int id;       // 任务的唯一标识符
    int (*func)(void *); // 任务函数指针
    void *arg;    // 任务参数
} Task;

// 线程池结构体
typedef struct thread_pool {
    int num_threads; // 线程池中的线程数量
    pthread_t *threads; // 线程数组
    sem_t task_available; // 信号量，用于通知线程有任务可用
    sem_t mutex; // 互斥锁，保护队列
    Task *queue; // 任务队列
    int head; // 队列头部索引
    int tail; // 队列尾部索引
    bool running; // 线程池是否正在运行
} ThreadPool;

// 初始化线程池
ThreadPool *thread_pool_init(int num_threads) {
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    pool->num_threads = num_threads;
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    pool->head = 0;
    pool->tail = 0;
    pool->running = true;
    sem_init(&pool->task_available, 0, 0);
    sem_init(&pool->mutex, 0, 1);
    pool->queue = malloc(num_threads * sizeof(Task));

    // 创建线程
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, thread_pool_thread, pool);
    }

    return pool;
}

// 线程池线程执行函数
void *thread_pool_thread(void *arg) {
    ThreadPool *pool = arg;
    while (pool->running) {
        sem_wait(&pool->task_available); // 等待任务可用信号
        sem_wait(&pool->mutex);          // 锁住互斥锁
        Task task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % pool->num_threads;
        sem_post(&pool->mutex);          // 解锁互斥锁
        task.func(task.arg);             // 执行任务
    }
    return NULL;
}

// 提交任务到线程池
void thread_pool_submit(ThreadPool *pool, int id, int (*func)(void *), void *arg) {
    sem_wait(&pool->mutex); // 锁住互斥锁
    pool->queue[pool->tail].id = id;
    pool->queue[pool->tail].func = func;
    pool->queue[pool->tail].arg = arg;
    pool->tail = (pool->tail + 1) % pool->num_threads;
    sem_post(&pool->mutex); // 解锁互斥锁
    sem_post(&pool->task_available); // 发送任务可用信号
}

// 关闭线程池
void thread_pool_destroy(ThreadPool *pool) {
    pool->running = false;
    sem_post(&pool->task_available); // 唤醒所有等待的线程
    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    free(pool->threads);
    free(pool->queue);
    sem_destroy(&pool->task_available);
    sem_destroy(&pool->mutex);
    free(pool);
}

// 示例任务函数
int example_task(void *arg) {
    int id = *(int *)arg;
    printf("Task %d started.\n", id);
    sleep(1); // 模拟耗时操作
    printf("Task %d finished.\n", id);
    return 0;
}

int main() {
    ThreadPool *pool = thread_pool_init(4); // 创建一个包含4个线程的线程池

    // 提交任务
    for (int i = 0; i < 10; i++) {
        int id = i;
        thread_pool_submit(pool, id, example_task, &id);
    }

    // 等待所有任务完成
    sleep(3); // 给线程池足够的时间来完成任务

    // 销毁线程池
    thread_pool_destroy(pool);

    return 0;
}