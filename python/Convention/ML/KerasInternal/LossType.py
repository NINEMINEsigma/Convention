from typing import *
from keras.api                  import losses

type losses_class_type = Union[
    losses.CTC,
    losses.BinaryCrossentropy,
    losses.BinaryFocalCrossentropy,
    losses.CategoricalCrossentropy,
    losses.CategoricalFocalCrossentropy,
    losses.CategoricalHinge,
    losses.Circle,
    losses.CosineSimilarity,
    losses.Dice,
    losses.Hinge,
    losses.Huber,
    losses.KLDivergence,
    losses.LogCosh,
    losses.MeanAbsoluteError,
    losses.MeanAbsolutePercentageError,
    losses.MeanSquaredError,
    losses.MeanSquaredLogarithmicError,
    losses.Poisson,
    losses.SparseCategoricalCrossentropy,
    losses.SquaredHinge,
    losses.Tversky,
]
type losses_func_type = Union[
    losses.binary_crossentropy,
    losses.binary_focal_crossentropy,
    losses.categorical_crossentropy,
    losses.categorical_focal_crossentropy,
    losses.categorical_hinge,
    losses.circle,
    losses.cosine_similarity,
    losses.ctc,
    losses.dice,
    losses.hinge,
    losses.huber,
    losses.kl_divergence,
    losses.log_cosh,
    losses.mean_absolute_error,
    losses.mean_absolute_percentage_error,
    losses.mean_squared_error,
    losses.mean_squared_logarithmic_error,
    losses.poisson,
    losses.sparse_categorical_crossentropy,
    losses.squared_hinge,
    losses.tversky,
]
losses_type = Union[
    losses.Loss,
    losses_class_type,
    losses_func_type,
    Callable[[Any, Any], Any]
    ]
'''
loss:
        Loss function. May be a string (name of loss function), or
        a `keras.losses.Loss` instance. See `keras.losses`. A
        loss function is any callable with the signature
        `loss = fn(y_true, y_pred)`, where `y_true` are the ground truth
        values, and `y_pred` are the model's predictions.
        `y_true` should have shape `(batch_size, d0, .. dN)`
        (except in the case of sparse loss functions such as
        sparse categorical crossentropy which expects integer arrays of
        shape `(batch_size, d0, .. dN-1)`).
        `y_pred` should have shape `(batch_size, d0, .. dN)`.
        The loss function should return a float tensor.
'''
