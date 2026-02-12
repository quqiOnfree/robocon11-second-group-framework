import os

def rename(directory = '.'):
    for filename in os.listdir(directory):
        if filename.endswith('.h'):
            new_name = filename.replace('.h', '.hpp')
            os.rename(os.path.join(directory, filename), os.path.join(directory, new_name))
        elif os.path.isdir(os.path.join(directory, filename)):
            rename(os.path.join(directory, filename))

if __name__ == "__main__":
    rename()
