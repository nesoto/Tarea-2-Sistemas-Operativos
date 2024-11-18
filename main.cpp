#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

class Monitor
{
private:
    std::vector<int> queue;
    size_t front, back, count, capacity;
    std::mutex mtx;
    std::condition_variable cv_producer, cv_consumer;
    std::atomic<bool> producers_done; // Bandera para indicar que los productores han terminado

    void resizeQueue(size_t new_capacity)
    {
        std::vector<int> new_queue(new_capacity);
        for (size_t i = 0; i < count; ++i)
        {
            new_queue[i] = queue[(front + i) % capacity];
        }
        queue = std::move(new_queue);
        front = 0;
        back = count;
        capacity = new_capacity;
        std::cout << "Cola redimensionada a: " << new_capacity << std::endl;
    }

public:
    Monitor(size_t initial_capacity) : front(0), back(0), count(0), capacity(initial_capacity), producers_done(false)
    {
        queue.resize(initial_capacity);
    }

    void setProducersDone()
    {
        producers_done = true;
        cv_consumer.notify_all(); // Notificar a todos los consumidores en espera
    }

    bool isQueueEmpty()
    {
        std::unique_lock<std::mutex> lock(mtx);
        return count == 0;
    }

    void produce(int item)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv_producer.wait(lock, [this]()
                         { return count < capacity; });

        queue[back] = item;
        back = (back + 1) % capacity;
        ++count;

        if (count == capacity)
        {
            resizeQueue(2 * capacity);
        }

        cv_consumer.notify_one();
    }

    int consume()
    {
        std::unique_lock<std::mutex> lock(mtx);
        // Esperar si la cola está vacía y los productores no han terminado
        cv_consumer.wait(lock, [this]()
                         { return count > 0 || producers_done; });

        if (count == 0 && producers_done)
        {
            return -1; // Indicador de que no hay más elementos por consumir
        }

        int item = queue[front];
        front = (front + 1) % capacity;
        --count;

        if (count > 0 && count <= capacity / 4)
        {
            resizeQueue(capacity / 2);
        }

        cv_producer.notify_one();
        return item;
    }
};

std::atomic<bool> producers_done(false);

// Función de los productores
void producerTask(Monitor &monitor, int num_items, int id)
{
    for (int i = 0; i < num_items; ++i)
    {
        monitor.produce(i + id * 1000);                              // Item único basado en el ID del productor
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulación de tiempo de producción
    }
    std::cout << "Productor " << id << " ha terminado de producir." << std::endl;
}

// Función de los consumidores
void consumerTask(Monitor &monitor, int wait_time, int id)
{
    auto start_time = std::chrono::steady_clock::now();
    while (true)
    {
        if (producers_done && monitor.isQueueEmpty())
        {
            break; // Salir si todos los productores han terminado y la cola está vacía
        }

        int item = monitor.consume();
        std::cout << "Consumidor " << id << " consumió: " << item << std::endl;

        // Verificar si se ha alcanzado el tiempo máximo de espera
        auto current_time = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count() > wait_time)
        {
            break; // Salir si ha pasado el tiempo de espera
        }
    }
    std::cout << "Consumidor " << id << " ha terminado de consumir." << std::endl;
}

int main(int argc, char *argv[])
{
    // Comprobar que se pasaron los argumentos correctos
    if (argc != 9)
    {
        std::cerr << "Uso: " << argv[0] << " -p <num_productores> -c <num_consumidores> -s <tamaño_inicial> -t <tiempo_espera>\n";
        return 1;
    }

    // Procesar los argumentos de la línea de comandos
    int num_producers = 0;
    int num_consumers = 0;
    size_t initial_capacity = 0;
    int max_wait_time = 0;

    for (int i = 1; i < argc; i += 2)
    {
        std::string arg = argv[i];
        if (arg == "-p")
        {
            num_producers = std::atoi(argv[i + 1]);
        }
        else if (arg == "-c")
        {
            num_consumers = std::atoi(argv[i + 1]);
        }
        else if (arg == "-s")
        {
            initial_capacity = std::atoi(argv[i + 1]);
        }
        else if (arg == "-t")
        {
            max_wait_time = std::atoi(argv[i + 1]);
        }
        else
        {
            std::cerr << "Argumento no reconocido: " << arg << "\n";
            return 1;
        }
    }

    // Inicializar monitor
    Monitor monitor(initial_capacity);

    std::vector<std::thread> producers;
    for (int i = 0; i < num_producers; ++i)
    {
        producers.emplace_back(producerTask, std::ref(monitor), 20, i);
    }

    std::vector<std::thread> consumers;
    for (int i = 0; i < num_consumers; ++i)
    {
        consumers.emplace_back(consumerTask, std::ref(monitor), max_wait_time, i);
    }

    for (auto &producer : producers)
    {
        producer.join();
    }

    // Señalar que los productores han terminado
    monitor.setProducersDone();

    for (auto &consumer : consumers)
    {
        consumer.join();
    }

    std::cout << "Simulación completa." << std::endl;
    return 0;
}