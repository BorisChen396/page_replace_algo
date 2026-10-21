import random

entries_count = random.randint(150000, 150100)
for _ in range(entries_count):
    print(random.randint(1, 1500), random.randint(0, 1), random.randint(0, 1))
