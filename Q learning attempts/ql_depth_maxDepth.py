import random
from itertools import product


class LayeredSortingNetworkEnv:
    def __init__(self, n_wires, max_depth):
        self.n_wires = n_wires
        self.max_depth = max_depth
        self.current_layers = [[]]
        self.all_possible_inputs = list(product([0, 1], repeat=n_wires))
        self.depth = 0

    def reset(self):
        self.current_layers = [[]]
        self.depth = 0
        return self._get_state_representation()

    def step(self, action, best_network_size):
        if action == "NEXT_LAYER":
            if self.depth + 1 < self.max_depth:
                self.current_layers.append([])
                self.depth += 1
                return self._get_state_representation(), -1, False
            else:
                return self._get_state_representation(), -100, True

        i, j = action
        if i > j:
            i, j = j, i

        current_layer = self.current_layers[self.depth]

        used = {x for comp in current_layer for x in comp}
        if i in used or j in used:
            return self._get_state_representation(), -10, False

        self.current_layers[self.depth].append((i, j))

        is_valid = self._check_sorting_network()
        total_comparators = sum(len(layer) for layer in self.current_layers)

        if is_valid:
            reward = 1000
            return self._get_state_representation(), reward, True

        elif self.depth >= self.max_depth - 1 and self._no_more_valid_moves():
            return self._get_state_representation(), -100, True
        else:
            return self._get_state_representation(), -1, False

    def _no_more_valid_moves(self):
        used = {x for comp in self.current_layers[self.depth] for x in comp}
        for i in range(self.n_wires):
            for j in range(i + 1, self.n_wires):
                if i not in used and j not in used:
                    return False
        return True

    def _get_state_representation(self):
        return tuple(tuple(layer) for layer in self.current_layers), self.depth

    def _check_sorting_network(self):
        for input_values in self.all_possible_inputs:
            values = list(input_values)

            for layer in self.current_layers:
                swaps = []
                for i, j in layer:
                    if values[i] > values[j]:
                        swaps.append((i, j))
                for i, j in swaps:
                    values[i], values[j] = values[j], values[i]

            if any(values[k] > values[k + 1] for k in range(len(values) - 1)):
                return False
        return True

    def get_valid_actions(self):
        used = {x for comp in self.current_layers[self.depth] for x in comp}
        actions = [
            (i, j) for i in range(self.n_wires) for j in range(i + 1, self.n_wires)
            if i not in used and j not in used
        ]
        actions.append("NEXT_LAYER")
        return actions

class QLearningAgent:
    def __init__(self, n_wires, max_depth, learning_rate=0.1, discount_factor=0.99, exploration_rate=0.95):
        self.env = LayeredSortingNetworkEnv(n_wires, max_depth)
        self.q_table = {}
        self.learning_rate = learning_rate
        self.discount_factor = discount_factor
        self.exploration_rate = exploration_rate
        self.valid_actions = self.env.get_valid_actions()
        self.n_wires = n_wires
        self.max_depth = max_depth

    def get_q_value(self, state, action):
        if (state, action) not in self.q_table:
            self.q_table[(state, action)] = 0.0
        return self.q_table[(state, action)]

    def choose_action(self, state):
        valid_actions = self.env.get_valid_actions()
        if not valid_actions:
            return "NEXT_LAYER"

        if random.uniform(0, 1) < self.exploration_rate:
            return random.choice(valid_actions)
        else:
            q_values = [self.get_q_value(state, a) for a in valid_actions]
            max_q = max(q_values)
            best_actions = [a for a, q in zip(valid_actions, q_values) if q == max_q]
            return random.choice(best_actions)


    def update_q_value(self, state, action, reward, next_state):
        next_valid_actions = self.env.get_valid_actions()
        max_next_q = (
            max(self.get_q_value(next_state, a) for a in next_valid_actions)
            if next_valid_actions else 0.0
        )

        current_q = self.get_q_value(state, action)
        new_q = current_q + self.learning_rate * (reward + self.discount_factor * max_next_q - current_q)
        self.q_table[(state, action)] = new_q


    def train(self, n_episodes=1000):
        best_network = None
        best_size = float('inf')

        for episode in range(n_episodes):
            state = self.env.reset()
            done = False

            while not done:
                action = self.choose_action(state)
                next_state, reward, done = self.env.step(action, best_size)
                self.update_q_value(state, action, reward, next_state)
                state = next_state

            total_comparators = sum(len(layer) for layer in self.env.current_layers)
            if reward > 0 and total_comparators < best_size:
                best_size = total_comparators
                best_network = [layer.copy() for layer in self.env.current_layers]
                print(f"episode {episode}: found new best with {best_size} comparators and {len(best_network)} layers")

            self.exploration_rate = max(0.01, self.exploration_rate * 0.9995)
            if episode % 100 == 0:
                print(f"episode {episode}, best: {best_size}, exploration rate: {self.exploration_rate:.4f}")

        return best_network, best_size

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

def find_layered_sorting_network(n_wires, max_depth, episodes=10000):
    agent = QLearningAgent(n_wires, max_depth)
    best_network, best_size = agent.train(n_episodes=episodes)
    return best_network, best_size

def convert_network_to_layers(network):
    return {depth: layer for depth, layer in enumerate(network)}


if __name__ == "__main__":
    n_wires = 10
    max_depth = n_wires * 2
    best_network, best_size = find_layered_sorting_network(n_wires, max_depth, episodes=10000)
    
    layers = convert_network_to_layers(best_network)

    print(f"\nFinal result for {n_wires} wires:")
    print(f"Min comparators: {best_size}")
    print(f"Network structure (layers):")
    plot_network(n_wires, layers)
