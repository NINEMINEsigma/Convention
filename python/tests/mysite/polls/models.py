from django.db import models
from django.utils import timezone

class Question(models.Model):
    """
    问题模型
    包含问题的文本和发布日期
    """
    question_text = models.CharField(max_length=200, verbose_name='问题文本')
    pub_date = models.DateTimeField('发布日期', default=timezone.now)

    def __str__(self):
        return self.question_text

    def was_published_recently(self):
        """
        判断问题是否最近发布（一天内）
        """
        return self.pub_date >= timezone.now() - timezone.timedelta(days=1)

class Choice(models.Model):
    """
    选项模型
    包含选项的文本和票数，与问题关联
    """
    question = models.ForeignKey(Question, on_delete=models.CASCADE, verbose_name='关联问题')
    choice_text = models.CharField(max_length=200, verbose_name='选项文本')
    votes = models.IntegerField(default=0, verbose_name='票数')

    def __str__(self):
        return self.choice_text 