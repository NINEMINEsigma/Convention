from django.shortcuts import render, get_object_or_404
from django.http import HttpResponse, HttpResponseRedirect
from django.urls import reverse
from django.views import generic
from .models import Question, Choice

# Create your views here.

class IndexView(generic.ListView):
    """
    首页视图
    """
    template_name = 'polls/index.html'
    context_object_name = 'latest_question_list'

    def get_queryset(self):
        return Question.objects.order_by('-pub_date')

class DetailView(generic.DetailView):
    """
    问题详情视图
    显示具体问题和选项
    """
    model = Question
    template_name = 'polls/detail.html'

class ResultsView(generic.DetailView):
    """
    结果视图
    显示投票结果
    """
    model = Question
    template_name = 'polls/results.html'

def vote(request, question_id):
    """
    处理投票请求
    """
    question = get_object_or_404(Question, pk=question_id)
    try:
        selected_choice = question.choice_set.get(pk=request.POST['choice'])
    except (KeyError, Choice.DoesNotExist):
        # 如果没有选择选项，重新显示问题详情页
        return render(request, 'polls/detail.html', {
            'question': question,
            'error_message': "请选择一个选项。",
        })
    else:
        selected_choice.votes += 1
        selected_choice.save()
        # 投票成功后重定向到结果页面
        return HttpResponseRedirect(reverse('polls:results', args=(question.id,)))
