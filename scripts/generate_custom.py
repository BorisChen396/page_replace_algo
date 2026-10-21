import random

N = random.randint(150000, 150100)
MAX_PAGE = 1500
LOOP_SIZE = 16
LOOP_REFS = (100, 200)
SCAN_REFS = (100, 300)

refs = []
scan_page = LOOP_SIZE + 1

while len(refs) < N:
    # Small range loop
    refs += random.choices(range(1, LOOP_SIZE + 1), k=random.randint(*LOOP_REFS))
    # Access a lot of pages, each page is used only once
    refs += random.sample(range(LOOP_SIZE + 1, MAX_PAGE), k=random.randint(*SCAN_REFS))

# Trim to N
refs = refs[:N]

for p in refs:
    print(p, random.randint(0, 1), random.randint(0, 1))
