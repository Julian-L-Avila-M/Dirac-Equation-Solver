class Geometry:
    """ 
    basis class for all geometrys
    """
    pass

class Interval1D(Geometry):
    """
    1D geometry define in a interval L
    """
    def __init__(self, x_min, x_max):
        self.min = x_min
        self.max = x_max

