import matplotlib.pyplot as plt
import math
from collections import defaultdict


def generate_network(n):
    layers = defaultdict(list)
    depth = 0
    def helper(lo, n):
        if n > 1:
            m = n // 2
            helper(lo, m)
            helper(lo + m, m)
            merge(lo, n, 1)

    def merge(lo, n, r):
        nonlocal depth
        step = r * 2
        if step < n:
            depth += 1
            merge(lo, n, step)
            merge(lo + r, n, step)
            for i in range(lo + r, lo + n - r, step):
                layers[int(int(math.log2(n)) * (int(math.log2(n))-1) / 2)+int(math.log2(n))-depth].append((i, i + r))
            depth -= 1
        else:
            layers[int(int(math.log2(n)) * (int(math.log2(n))-1) / 2)].append((lo, lo + r))

    helper(0, n)
    return layers

def plot_network(n, layers):
    import matplotlib.pyplot as plt

    fig, ax = plt.subplots(figsize=(12, 0.6 * n))

    depth_spacing = 0.5
    micro_offset = 0.15
    wire_labels_padding = 0.15
    wire_start_offset = 0.5
    initial_layer_offset = 0.25
    wire_end_margin = 0.25

    comparator_draws = []

    x_pos = wire_start_offset + initial_layer_offset
    for depth in sorted(layers.keys()):
        comps = sorted(layers[depth], key=lambda x: (x[0], x[1]))
        placed = []
        max_offset = 0

        for a, b in comps:
            offset = 0
            while any((min(a, b) <= max(c, d) and max(a, b) >= min(c, d)) and offset == used_offset
                      for (c, d, used_offset) in placed):
                offset += 1
            placed.append((a, b, offset))
            max_offset = max(max_offset, offset)

        for a, b, offset in placed:
            y1 = n - 1 - a
            y2 = n - 1 - b
            x = x_pos + offset * micro_offset
            comparator_draws.append((x, y1, y2))

        x_pos += depth_spacing + max_offset * micro_offset

    final_x_pos = x_pos

    for i in range(n):
        y = n - 1 - i
        ax.plot([wire_start_offset, final_x_pos + wire_end_margin], [y, y], color='black', linewidth=1)
        ax.text(wire_start_offset - wire_labels_padding, y, f"{i}", va='center', ha='right', fontsize=10)

    for x, y1, y2 in comparator_draws:
        ax.plot([x, x], [y1, y2], color='red', linewidth=2)

    ax.set_xlim(0, final_x_pos + wire_end_margin + 0.5)
    ax.set_ylim(-1, n)
    ax.set_title(f'Odd-Even Merge Sort Network for n={n}\nComparators: {sum(len(c) for c in layers.values())}, Depth: {len(layers)}',
                 fontsize=14)
    ax.axis('off')
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    n = 8
    layers = generate_network(n)
    plot_network(n, layers)
    
    total_comparators = sum(len(comps) for comps in layers.values())
    depth = len(layers)
    print(f"Total comparators: {total_comparators}")
    print(f"Depth: {depth}")
