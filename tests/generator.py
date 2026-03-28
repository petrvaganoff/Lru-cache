#!/usr/bin/env python3

from pathlib import Path
import random


def generate_values(count: int) -> list[int]:
    return [random.randint(-2147483648, 2147483647) for _ in range(count)]


def write_testcase(path: Path, capacity: int, values: list[int]) -> None:
    content = (
        f"{capacity}\n"
        f"{len(values)}\n"
        f"{' '.join(map(str, values))}\n"
    )
    path.write_text(content)


def main() -> None:
    random.seed(42)

    for i in range(1, 101):
        capacity = random.randint(1, 100000)
        count = random.randint(5, 1000000)

        values = generate_values(count)

        file_path = Path(f'case{i:02d}')
        write_testcase(file_path, capacity, values)


if __name__ == "__main__":
    main()
