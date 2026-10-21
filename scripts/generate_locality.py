import random

N = random.randint(150000, 150100)
MAX_PAGE = 1500
RANGE_SIZE_BOUND = (50, 100)

refs = []
while len(refs) < N:
    range_size = random.randint(*RANGE_SIZE_BOUND)
    start_page_num = random.randint(1, MAX_PAGE - range_size + 1)
    refs += random.choices(range(start_page_num, start_page_num + range_size), k=random.randint(N // 200, N // 100))

# Trim to N
refs = refs[:N]

for p in refs:
    print(p, random.randint(0, 1), random.randint(0, 1))
