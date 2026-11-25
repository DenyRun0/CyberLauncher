# Протокол локальної перевірки — 25.11.2026

## Негативні сценарії

Середовище: Linux, g++ 14.2.0, C++17. Використано справжній `modstate.hpp` з GitHub; SHA Git blob перевірено: `3d2a0fb8da683ac8e214bbf8c8f27c1047fb74ee`. Тест компілювався командою:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic tests/modstate_negative_tests.cpp -o /tmp/cyber-negative-tests
/tmp/cyber-negative-tests
```

Фактичний вивід:

```text
PASS: empty filename
PASS: unsupported extension
PASS: missing mod file
PASS: directory instead of file
PASS: path outside mods directory
```

Код завершення: 0. 5/5. Відсутність небажаних змін у тестовій папці також перевірено. Нова ціль CTest — modstate_negative_tests. Не виконувалося графічне тестування лаунчера.

## Локальні Git-команди

Команди виконано у локальному навчальному Git-репозиторії зі знімком реального модуля `modstate.hpp` (blob `3d2a0fb8da683ac8e214bbf8c8f27c1047fb74ee`). Віддалене сховище у цьому експерименті — локальний bare-репозиторій. Прямий доступ до github.com із середовища заблокований DNS, тому публікація в основному репозиторії виконується окремо через GitHub API.

```text
git version 2.47.3
Cloning into 'work'...
done.
Already up to date.
Switched to a new branch 'demo/negative-tests'
[demo/negative-tests bb56f52] Додано негативні перевірки модуля
 * [new branch]      demo/negative-tests -> demo/negative-tests
branch 'demo/negative-tests' set up to track 'origin/demo/negative-tests'.
bb56f52 Додано негативні перевірки модуля
bb93514 Початковий знімок модуля
```
Команди clone, pull, switch, add, commit, push, log, status і branch виконано успішно; робоче дерево чисте. 

## Статичний аналіз інших негативних сценаріїв

- mainwindow.cpp: on_playButton_clicked повертається при порожньому нікнеймі або версії; це спостереження з коду, а не GUI-тест.
- Перевірки формату нікнейму за регулярним виразом у цьому обробнику немає. Негативний тест для спеціальних символів не можна позначити PASS.
- core.hpp: інсталяція мода відхиляє порожню відповідь сумісних версій; GUI-сценарій не виконано.
- mainwindow.cpp: кнопки блокуються під час запуску/гри; повторне натискання не перевірялося вручну.

Учасник окремо підтвердив запуск Intel Mac і роботу кнопки перемикання модів. Інші ручні сценарії не позначені як пройдені.
