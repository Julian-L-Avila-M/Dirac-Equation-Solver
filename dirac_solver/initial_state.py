import numpy as np

class InitialState:
    """
    basis class for all states
    """
    pass

class PlaneWave(InitialState):
    """
    make planewave initialstate (for 1+1D free particle dirac)
    """
    pass

    def __init__(self):
        print("planewave created")
    
    def create_state(self):
        """
        for create state
        """
        print("state created")



