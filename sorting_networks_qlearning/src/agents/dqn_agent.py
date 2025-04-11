import random
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers


class DQNAgent:
    def __init__(self, env, state_size, action_size,
                 epsilon=1.0, epsilon_decay=0.995, epsilon_min=0.01,
                 learning_rate=0.001, discount=0.99):
        self.env = env
        self.state_size = state_size
        self.action_size = action_size
        self.epsilon = epsilon
        self.epsilon_decay = epsilon_decay
        self.epsilon_min = epsilon_min
        self.learning_rate = learning_rate
        self.discount = discount

        self.model = self.build_model()
        self.target_model = self.build_model()
        self.update_target_model()
        self.memory = []  # Replay memory

    def build_model(self):
        model = tf.keras.Sequential()
        model.add(layers.Dense(24, input_dim=self.state_size, activation='relu'))
        model.add(layers.Dense(24, activation='relu'))
        model.add(layers.Dense(self.action_size, activation='linear'))
        model.compile(loss='mse', optimizer=tf.keras.optimizers.Adam(learning_rate=self.learning_rate))

        print("2. agents dqn build model")

        return model

    def update_target_model(self):
        self.target_model.set_weights(self.model.get_weights())
        print("3. agents dqn update target model")

    def remember(self, state, action, reward, next_state, done):
        self.memory.append((state, action, reward, next_state, done))
        print("4. agents dqn remember memory")

    def select_action(self, state):
        if np.random.rand() <= self.epsilon:
            return random.randrange(self.action_size)
        state_input = np.array(state).reshape(1, -1)
        q_values = self.model.predict(state_input, verbose=0)

        print("5. agents dqn select action")

        return np.argmax(q_values[0])

    def replay(self, batch_size):
        if len(self.memory) < batch_size:
            return
        minibatch = random.sample(self.memory, batch_size)
        for state, action, reward, next_state, done in minibatch:
            target = reward
            state_input = np.array(state).reshape(1, -1)
            next_state_input = np.array(next_state).reshape(1, -1)
            if not done:
                target = reward + self.discount * np.amax(self.target_model.predict(next_state_input, verbose=0)[0])
            target_f = self.model.predict(state_input, verbose=0)
            target_f[0][action] = target
            self.model.fit(state_input, target_f, epochs=1, verbose=0)
        if self.epsilon > self.epsilon_min:
            self.epsilon *= self.epsilon_decay
        print("6. agents dqn replay memory")

    def save_checkpoint(self, filename):
        self.model.save_weights(filename)
        print("7. agents dqn save checkpoint")

    def load_checkpoint(self, filename):
        self.model.load_weights(filename)
        print("8. agents dqn load checkpoint")
