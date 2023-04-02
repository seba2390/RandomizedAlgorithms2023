import numpy as np
from scipy.special import erfc


def remove_outliers_IQR(arr: np.ndarray, k: float = 1.5) -> tuple[np.ndarray, np.ndarray]:
    """
    Removes outliers from a 1D NumPy array using the IQR method.

    Parameters:
    -----------
    arr : numpy.ndarray
        A 1D NumPy array of values.
    k : float, optional (default=1.5)
        A constant that determines the range of the outliers.

    Returns:
    --------
    tuple : (numpy.ndarray, numpy.ndarray)
        A tuple containing two NumPy arrays:
        - A 1D NumPy array of the original values with the outliers removed.
        - A 1D NumPy array of the removed outliers.
    """

    # Calculate the quartiles and IQR
    q1 = np.percentile(arr, 25)
    q3 = np.percentile(arr, 75)
    iqr = q3 - q1

    # Calculate the lower and upper bounds
    lower_bound = q1 - k * iqr
    upper_bound = q3 + k * iqr

    # Find the indices of the outliers
    outlier_indices = np.where((arr < lower_bound) | (arr > upper_bound))[0]

    # Remove the outliers from the original array
    filtered_arr = np.delete(arr, outlier_indices)

    # Create a separate array of the removed outliers
    outliers = arr[outlier_indices]

    return filtered_arr, outliers


def remove_outliers_Z(arr: np.ndarray, threshold: float = 3.0) -> tuple[np.ndarray, np.ndarray]:
    """
    Removes outliers from a 1D NumPy array using the Z-score method.

    Parameters:
    -----------
    arr : numpy.ndarray
        A 1D NumPy array of values.
    threshold : float, optional (default=3.0)
        The threshold value for the Z-score. Any data point with a Z-score
        greater than this value is considered an outlier.

    Returns:
    --------
    tuple : (numpy.ndarray, numpy.ndarray)
        A tuple containing two NumPy arrays:
        - A 1D NumPy array of the original values with the outliers removed.
        - A 1D NumPy array of the removed outliers.
    """

    # Calculate the mean and standard deviation of the array
    mean = np.mean(arr)
    std = np.std(arr)

    # Calculate the Z-score for each value in the array
    z_scores = np.abs((arr - mean) / std)

    # Find the indices of the outliers
    outlier_indices = np.where(z_scores > threshold)[0]

    # Remove the outliers from the original array
    filtered_arr = np.delete(arr, outlier_indices)

    # Create a separate array of the removed outliers
    outliers = arr[outlier_indices]

    return filtered_arr, outliers


def remove_outliers_tukey(arr: np.ndarray, k: float = 1.5) -> tuple[np.ndarray, np.ndarray]:
    """
    Removes outliers from a 1D NumPy array using Tukey's fences method.

    Parameters:
    -----------
    arr : numpy.ndarray
        A 1D NumPy array of values.

    Returns:
    --------
    tuple : (numpy.ndarray, numpy.ndarray)
        A tuple containing two NumPy arrays:
        - A 1D NumPy array of the original values with the outliers removed.
        - A 1D NumPy array of the removed outliers.
    """

    # Calculate the interquartile range (IQR) of the array
    q1, q3 = np.percentile(arr, [25, 75])
    iqr = q3 - q1

    # Calculate the lower and upper bounds using Tukey's fences
    lower_bound = q1 - k * iqr
    upper_bound = q3 + k * iqr

    # Identify the outliers
    outlier_indices = np.where((arr < lower_bound) | (arr > upper_bound))[0]

    # Remove the outliers from the original array
    filtered_arr = np.delete(arr, outlier_indices)

    # Create a separate array of the removed outliers
    outliers = arr[outlier_indices]

    return filtered_arr, outliers


def remove_outliers_chauvenet(arr: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """
    Removes outliers from a 1D NumPy array using Chauvenet's criterion.

    Parameters:
    -----------
    arr : numpy.ndarray
        A 1D NumPy array of values.

    Returns:
    --------
    tuple : (numpy.ndarray, numpy.ndarray)
        A tuple containing two NumPy arrays:
        - A 1D NumPy array of the original values with the outliers removed.
        - A 1D NumPy array of the removed outliers.
    """

    # Calculate the arithmetic mean and standard deviation of the array
    mean = np.mean(arr)
    std_dev = np.std(arr, ddof=1)  # Use the sample standard deviation

    # Calculate the z-score for each value in the array
    z_scores = np.abs((arr - mean)) / std_dev

    # Identify the outliers
    outlier_indices = np.where(erfc(z_scores) < 1 / (2 * len(arr)))[0]

    # Remove the outliers from the original array
    filtered_arr = np.delete(arr, outlier_indices)

    # Create a separate array of the removed outliers
    outliers = arr[outlier_indices]

    return filtered_arr, outliers
