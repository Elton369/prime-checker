# Prime Checker

Проект по курсу «Технологии распространения, развертывания и сопровождения ПО».  
Выполнены практические работы №1–4 (вариант 2).

---

## Описание

Приложение на C++, которое:
- проверяет число на простоту;
- работает как HTTP-сервис;
- предоставляет метрики для Prometheus;
- разворачивается в Docker и Kubernetes;
- собирается и публикуется через CI/CD pipeline.

---

## Функциональность

Доступные endpoint'ы:

- GET /health — проверка доступности
- GET /prime?n=17 — проверка числа
- GET /metrics — метрики Prometheus

### Примеры

```bash
curl http://127.0.0.1:8080/health
curl "http://127.0.0.1:8080/prime?n=17"
curl http://127.0.0.1:8080/metrics
```

---

## Структура проекта

```text
prime-checker/
├── .github/workflows/ci.yml
├── cicd/
│   ├── run_make.sh
│   ├── run_tests.sh
│   ├── run_package.sh
│   ├── run_docker.sh
│   └── run_release.sh
├── DEBIAN/control
├── k8s/
│   ├── deployment.yaml
│   ├── namespace.yaml
│   ├── service.yaml
│   └── servicemonitor.yaml
├── src/
│   ├── main.cpp
│   └── Makefile
├── usr/bin/.gitkeep
├── .gitignore
├── Dockerfile
└── README.md
```

---

# Практическая работа №1

## Что сделано

- разработана программа на C++;
- сборка через make;
- проверка компилятора;
- создан .deb пакет;
- учтены зависимости.

## Сборка

```bash
cd src
make clean
make
cd ..
```

## Запуск

```bash
./usr/bin/prime-checker
```

## Сборка .deb

```bash
chmod +x cicd/run_package.sh
./cicd/run_package.sh
```

---

# Практическая работа №2 (CI/CD)

## Что сделано

- настроен pipeline;
- этапы: build → test → package → docker → release;
- добавлены тесты;
- публикация .deb в релизах.

## Локальная проверка

```bash
chmod +x cicd/run_make.sh
./cicd/run_make.sh

chmod +x cicd/run_tests.sh
./cicd/run_tests.sh

chmod +x cicd/run_package.sh
./cicd/run_package.sh
```

---

# Практическая работа №3 (Docker)

## Что сделано

- создан Dockerfile;
- приложение упаковано в контейнер;
- добавлен docker-этап в CI;
- образ публикуется в GHCR.

## Сборка контейнера

```bash
docker build -t prime-checker:test .
```

## Запуск контейнера

```bash
docker run -d --name prime-checker-test -p 8080:8080 prime-checker:test
```

## Проверка

```bash
curl http://127.0.0.1:8080/health
curl "http://127.0.0.1:8080/prime?n=11"
curl http://127.0.0.1:8080/metrics
```

## Остановка

```bash
docker stop prime-checker-test
docker rm prime-checker-test
```

## Docker образ

```bash
docker pull ghcr.io/elton369/prime-checker:latest
docker pull ghcr.io/elton369/prime-checker:v1.1.0
```

---

# Практическая работа №4 (Kubernetes и мониторинг)

## Что сделано

- создан Deployment;
- создан Service;
- приложение развёрнуто в Kubernetes;
- установлен Prometheus;
- настроен сбор метрик;
- добавлен ServiceMonitor.

## Развертывание

```bash
kubectl apply -f k8s/namespace.yaml
kubectl apply -f k8s/deployment.yaml
kubectl apply -f k8s/service.yaml
```

## Проверка

```bash
kubectl get pods -n prime-checker
kubectl get svc -n prime-checker
```

## Доступ к сервису

```bash
kubectl port-forward -n prime-checker svc/prime-checker-service 8080:8080
```

```bash
curl http://127.0.0.1:8080/health
```

## Установка Prometheus

```bash
helm repo add prometheus-community https://prometheus-community.github.io/helm-charts
helm repo update

helm install kube-prometheus-stack prometheus-community/kube-prometheus-stack \
  --namespace monitoring \
  --create-namespace
```

## Подключение мониторинга

```bash
kubectl apply -f k8s/servicemonitor.yaml
```

## Проверка метрик

```bash
curl http://127.0.0.1:8080/metrics
```

Пример:

```text
prime_checker_requests_total
prime_checker_prime_requests_total
```

---

## Технологии

- C++
- Make
- Debian (.deb)
- GitHub Actions (CI/CD)
- Docker
- GitHub Container Registry (GHCR)
- Kubernetes
- Prometheus
