import argparse
from environment.simulation import SortingNetworkEnv
from agents.qlearning_agent import QLearningAgent
from utils.logger import setup_logger


def train(args):
    logger = setup_logger(args.log_file)
    env = SortingNetworkEnv(n=args.n) # creare instanta a mediului de sortare pt nr specificat de fire
    agent = QLearningAgent(env, epsilon=args.epsilon, learning_rate=args.alpha, discount=args.gamma) # instanta agent - primeste mediul ca arg

    num_episodes = args.episodes
    for episode in range(num_episodes):
        state = env.reset()
        done = False # pt final episod
        total_reward = 0

        while not done:
            action = agent.select_action(state) # agentul selecteaza o actiune - strategie = epsilon-greedy
            next_state, reward, done, info = env.step(action) # mediul exect act => modif ret
            agent.update(state, action, reward, next_state, done) # update Q folosind Q(s,a) = Q(s,a) + alpha * [reward + gamma * max_a'(Q(s',a')) - Q(s,a)]
            state = next_state
            total_reward += reward

        logger.info(f"Episode {episode + 1}/{num_episodes}, Reward: {total_reward}")
        if (episode + 1) % args.checkpoint_interval == 0:
            agent.save_checkpoint(f"checkpoint_{episode + 1}.pth")
    print("TRAINING FINISHED")
    logger.info("Training completed.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--n", type=int, default=4, help="Numărul de fire din sorting network")
    parser.add_argument("--epsilon", type=float, default=0.1, help="Epsilon pentru strategia epsilon-greedy")
    parser.add_argument("--alpha", type=float, default=0.01, help="Rata de învățare")
    parser.add_argument("--gamma", type=float, default=0.99, help="Factorul de discount")
    parser.add_argument("--episodes", type=int, default=1000, help="Numărul de episoade de antrenament")
    parser.add_argument("--checkpoint_interval", type=int, default=100, help="Intervalul pentru checkpointing")
    parser.add_argument("--log_file", type=str, default="training.log", help="Numele fișierului de log")
    args = parser.parse_args()
    train(args)
