import logging

class Logger:
  def __init__(self, Program):
    self.logger = logging.getLogger(Program)
    self.logger.setLevel(logging.DEBUG)

    file = "../log/" + Program + ".log"
    fh = logging.FileHandler(file)
    fh.setLevel(logging.DEBUG)

    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

    formatter = logging.Formatter('%(levelname)s - %(asctime)s - %(message)s')
    fh.setFormatter(formatter)
    ch.setFormatter(formatter)

    self.logger.addHandler(fh)
    self.logger.addHandler(ch)
