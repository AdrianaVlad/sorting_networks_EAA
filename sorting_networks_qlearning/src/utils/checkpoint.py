import pickle

def save_checkpoint(obj, filename):
    with open(filename, "wb") as f:
        pickle.dump(obj, f)
    print("28")

def load_checkpoint(filename):
    with open(filename, "rb") as f:
        return pickle.load(f)
    print("29")
