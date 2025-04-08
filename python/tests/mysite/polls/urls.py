from django.urls import path
from . import views

app_name = 'polls'
urlpatterns = [
    # 首页
    path('', views.IndexView.as_view(), name='index'),
    # 问题详情页
    path('<int:pk>/', views.DetailView.as_view(), name='detail'),
    # 结果页
    path('<int:pk>/results/', views.ResultsView.as_view(), name='results'),
    # 投票处理
    path('<int:question_id>/vote/', views.vote, name='vote'),
] 