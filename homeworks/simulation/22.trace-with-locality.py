import math
import random


def generate_trace(
    num_references: int = 100,
    num_pages: int = 100,
    hot_pages_rate: float = 0.2,
    hot_references_rate: float = 0.8,
) -> list[int]:
    """Generate memory references such that most references go to hot pages."""
    # Assume the 1st pages are hot pages for simplicity
    num_hot_pages = math.ceil(hot_pages_rate * num_pages)
    hot_pages = [i for i in range(1, num_hot_pages + 1)]
    cold_pages = [i for i in range(num_hot_pages + 1, num_pages + 1)]

    # Generate references to hot/cold pages
    references: list[int] = []
    for _ in range(num_references):
        is_hot = random.random() < hot_references_rate
        if is_hot:
            references.append(random.choice(hot_pages))
        else:
            references.append(random.choice(cold_pages))

    return references


if __name__ == "__main__":
    trace = generate_trace()
    print(",".join(str(addr) for addr in trace))
